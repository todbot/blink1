using System;
using System.Runtime.InteropServices;
using System.Threading;

namespace HidLibrary
{
    public class HidDevice : IDisposable
    {
        public event InsertedEventHandler Inserted;
        public event RemovedEventHandler Removed;

        public delegate void InsertedEventHandler();
        public delegate void RemovedEventHandler();

        public enum DeviceMode
        {
            NonOverlapped = 0,
            Overlapped = 1
        }

        private readonly string _devicePath;
        private readonly HidDeviceAttributes _deviceAttributes;

        private readonly HidDeviceCapabilities _deviceCapabilities;
        private DeviceMode _deviceReadMode = DeviceMode.NonOverlapped;
        private DeviceMode _deviceWriteMode = DeviceMode.NonOverlapped;

        private readonly HidDeviceEventMonitor _deviceEventMonitor;
        
        private bool _monitorDeviceEvents;
        private delegate HidDeviceData ReadDelegate();
        private delegate HidReport ReadReportDelegate();
        private delegate bool WriteDelegate(byte[] data);
        private delegate bool WriteReportDelegate(HidReport report);

        public delegate void ReadCallback(HidDeviceData data);
        public delegate void ReadReportCallback(HidReport report);

        public delegate void WriteCallback(bool success);

        internal HidDevice(string devicePath)
        {
            _deviceEventMonitor = new HidDeviceEventMonitor(this);
            _deviceEventMonitor.Inserted += DeviceEventMonitorInserted;
            _deviceEventMonitor.Removed += DeviceEventMonitorRemoved;

            _devicePath = devicePath;

            try
            {
                var hidHandle = OpenDeviceIO(_devicePath, NativeMethods.ACCESS_NONE);

                _deviceAttributes = GetDeviceAttributes(hidHandle);
                _deviceCapabilities = GetDeviceCapabilities(hidHandle);

                CloseDeviceIO(hidHandle);
            }
            catch (Exception exception)
            {
                throw new Exception(string.Format("Error querying HID device '{0}'.", devicePath), exception);
            }
        }

        public IntPtr ReadHandle { get; private set; }
        public IntPtr WriteHandle { get; private set; }
        public bool IsOpen { get; private set; }
        public bool IsConnected { get { return HidDevices.IsConnected(_devicePath); } }
        public string Description { get { return ToString(); } }
        public HidDeviceCapabilities Capabilities { get { return _deviceCapabilities; } }
        public HidDeviceAttributes Attributes { get { return _deviceAttributes; } }
        public string DevicePath { get { return _devicePath; } }

        public bool MonitorDeviceEvents
        {
            get { return _monitorDeviceEvents; }
            set
            {
                if (value & _monitorDeviceEvents == false) _deviceEventMonitor.Init();
                _monitorDeviceEvents = value;
            }
        }

        public override string ToString()
        {
            return string.Format("VendorID={0}, ProductID={1}, Version={2}, DevicePath={3}", 
                                _deviceAttributes.VendorHexId,
                                _deviceAttributes.ProductHexId,
                                _deviceAttributes.Version,
                                _devicePath);
        }

        public void OpenDevice()
        {
            OpenDevice(DeviceMode.NonOverlapped, DeviceMode.NonOverlapped);
        }

        public void OpenDevice(DeviceMode readMode, DeviceMode writeMode)
        {
            if (IsOpen) return;

            _deviceReadMode = readMode;
            _deviceWriteMode = writeMode;

            try
            {
                ReadHandle = OpenDeviceIO(_devicePath, readMode, NativeMethods.GENERIC_READ);
                WriteHandle = OpenDeviceIO(_devicePath, writeMode, NativeMethods.GENERIC_WRITE);
            }
            catch (Exception exception)
            {
                IsOpen = false;
                throw new Exception("Error opening HID device.", exception);
            }

            IsOpen = ReadHandle.ToInt32() != NativeMethods.INVALID_HANDLE_VALUE & WriteHandle.ToInt32() != NativeMethods.INVALID_HANDLE_VALUE;
        }


        public void CloseDevice()
        {
            if (!IsOpen) return;
            CloseDeviceIO(ReadHandle);
            CloseDeviceIO(WriteHandle);
            IsOpen = false;
        }

        public HidDeviceData Read()
        {
            return Read(0);
        }

        public void Read(ReadCallback callback)
        {
            var readDelegate = new ReadDelegate(Read);
            var asyncState = new HidAsyncState(readDelegate, callback);
            readDelegate.BeginInvoke(EndRead, asyncState);
        }

        public HidDeviceData Read(int timeout)
        {
            if (IsConnected)
            {
                if (IsOpen == false) OpenDevice();
                try
                {
                    return ReadData(timeout);
                }
                catch
                {
                    return new HidDeviceData(HidDeviceData.ReadStatus.ReadError);
                }

            }
            return new HidDeviceData(HidDeviceData.ReadStatus.NotConnected);
        }

        public void ReadReport(ReadReportCallback callback)
        {
            var readReportDelegate = new ReadReportDelegate(ReadReport);
            var asyncState = new HidAsyncState(readReportDelegate, callback);
            readReportDelegate.BeginInvoke(EndReadReport, asyncState);
        }

        public HidReport ReadReport(int timeout)
        {
            return new HidReport(Capabilities.InputReportByteLength, Read(timeout));
        }

        public HidReport ReadReport()
        {
            return ReadReport(0);
        }

        public void Write(byte[] data, WriteCallback callback)
        {
            var writeDelegate = new WriteDelegate(Write);
            var asyncState = new HidAsyncState(writeDelegate, callback);
            writeDelegate.BeginInvoke(data, EndWrite, asyncState);
        }

        public bool Write(byte[] data)
        {
            return Write(data, 0);
        }

        public bool Write(byte[] data, int timeout)
        {
            if (IsConnected)
            {
                if (IsOpen == false) OpenDevice();
                try
                {
                    return WriteData(data, timeout);
                }
                catch
                {
                    return false;
                }
            }
            return false;
        }

        public void WriteReport(HidReport report, WriteCallback callback)
        {
            var writeReportDelegate = new WriteReportDelegate(WriteReport);
            var asyncState = new HidAsyncState(writeReportDelegate, callback);
            writeReportDelegate.BeginInvoke(report, EndWriteReport, asyncState);
        }

        public bool WriteReport(HidReport report)
        {
            return WriteReport(report, 0);
        }

        public bool WriteReport(HidReport report, int timeout)
        {
            return Write(report.GetBytes(), timeout);
        }

        public HidReport CreateReport()
        {
            return new HidReport(Capabilities.OutputReportByteLength);
        }


        public bool ReadFeatureData(byte reportId, out byte[] data)
        {
            if (_deviceCapabilities.FeatureReportByteLength <= 0)
            {
                data = new byte[0];
                return false;
            }

            data = new byte[_deviceCapabilities.FeatureReportByteLength];

            var buffer = CreateFeatureInputBuffer();
            buffer[0] = reportId;

            IntPtr hidHandle = IntPtr.Zero;
            bool success = false;
            try
            {
                hidHandle = OpenDeviceIO(_devicePath, NativeMethods.ACCESS_NONE);

                success = NativeMethods.HidD_GetFeature(hidHandle, buffer, buffer.Length);
                Array.Copy(buffer, 0, data, 0, Math.Min(data.Length, _deviceCapabilities.FeatureReportByteLength));
            }
            catch (Exception exception)
            {
                throw new Exception(string.Format("Error accessing HID device '{0}'.", _devicePath), exception);
            }
            finally
            {
                if (hidHandle != IntPtr.Zero)
                    CloseDeviceIO(hidHandle);
            }

            return success;
        }

        public bool WriteFeatureData(byte[] data)
        {
            if (_deviceCapabilities.FeatureReportByteLength <= 0) return false;

            var buffer = CreateFeatureOutputBuffer();

            Array.Copy(data, 0, buffer, 0, Math.Min(data.Length, _deviceCapabilities.FeatureReportByteLength));


            IntPtr hidHandle = IntPtr.Zero;
            bool success = false;
            try
            {
                hidHandle = OpenDeviceIO(_devicePath, NativeMethods.ACCESS_NONE);

                var overlapped = new NativeOverlapped();
                success = NativeMethods.HidD_SetFeature(hidHandle, buffer, buffer.Length);
            }
            catch (Exception exception)
            {
                throw new Exception(string.Format("Error accessing HID device '{0}'.", _devicePath), exception);
            }
            finally
            {
                if (hidHandle != IntPtr.Zero)
                    CloseDeviceIO(hidHandle);
            }
            return success;
        }

        private static void EndRead(IAsyncResult ar)
        {
            var hidAsyncState = (HidAsyncState)ar.AsyncState;
            var callerDelegate = (ReadDelegate)hidAsyncState.CallerDelegate;
            var callbackDelegate = (ReadCallback)hidAsyncState.CallbackDelegate;
            var data = callerDelegate.EndInvoke(ar);

            if ((callbackDelegate != null)) callbackDelegate.Invoke(data);
        }

        private static void EndReadReport(IAsyncResult ar)
        {
            var hidAsyncState = (HidAsyncState)ar.AsyncState;
            var callerDelegate = (ReadReportDelegate)hidAsyncState.CallerDelegate;
            var callbackDelegate = (ReadReportCallback)hidAsyncState.CallbackDelegate;
            var report = callerDelegate.EndInvoke(ar);

            if ((callbackDelegate != null)) callbackDelegate.Invoke(report);
        }

        private static void EndWrite(IAsyncResult ar)
        {
            var hidAsyncState = (HidAsyncState)ar.AsyncState;
            var callerDelegate = (WriteDelegate)hidAsyncState.CallerDelegate;
            var callbackDelegate = (WriteCallback)hidAsyncState.CallbackDelegate;
            var result = callerDelegate.EndInvoke(ar);

            if ((callbackDelegate != null)) callbackDelegate.Invoke(result);
        }

        private static void EndWriteReport(IAsyncResult ar)
        {
            var hidAsyncState = (HidAsyncState)ar.AsyncState;
            var callerDelegate = (WriteReportDelegate)hidAsyncState.CallerDelegate;
            var callbackDelegate = (WriteCallback)hidAsyncState.CallbackDelegate;
            var result = callerDelegate.EndInvoke(ar);

            if ((callbackDelegate != null)) callbackDelegate.Invoke(result);
        }

        private byte[] CreateInputBuffer()
        {
            return CreateBuffer(Capabilities.InputReportByteLength - 1);
        }

        private byte[] CreateOutputBuffer()
        {
            return CreateBuffer(Capabilities.OutputReportByteLength - 1);
        }

        private byte[] CreateFeatureOutputBuffer()
        {
            return CreateBuffer(Capabilities.FeatureReportByteLength - 1);
        }

        private byte[] CreateFeatureInputBuffer()
        {
            return CreateBuffer(Capabilities.FeatureReportByteLength - 1);
        }

        private static byte[] CreateBuffer(int length)
        {
            byte[] buffer = null;
            Array.Resize(ref buffer, length + 1);
            return buffer;
        }

        private static HidDeviceAttributes GetDeviceAttributes(IntPtr hidHandle)
        {
            var deviceAttributes = default(NativeMethods.HIDD_ATTRIBUTES);
            deviceAttributes.Size = Marshal.SizeOf(deviceAttributes);
            NativeMethods.HidD_GetAttributes(hidHandle, ref deviceAttributes);
            return new HidDeviceAttributes(deviceAttributes);
        }

        private static HidDeviceCapabilities GetDeviceCapabilities(IntPtr hidHandle)
        {
            var capabilities = default(NativeMethods.HIDP_CAPS);
            var preparsedDataPointer = default(IntPtr);

            if (NativeMethods.HidD_GetPreparsedData(hidHandle, ref preparsedDataPointer))
            {
                NativeMethods.HidP_GetCaps(preparsedDataPointer, ref capabilities);
                NativeMethods.HidD_FreePreparsedData(preparsedDataPointer);
            }
            return new HidDeviceCapabilities(capabilities);
        }

        private bool WriteData(byte[] data, int timeout)
        {
            if (_deviceCapabilities.OutputReportByteLength <= 0) return false;

            var buffer = CreateOutputBuffer();
            uint bytesWritten = 0;

            Array.Copy(data, 0, buffer, 0, Math.Min(data.Length, _deviceCapabilities.OutputReportByteLength));

            if (_deviceWriteMode == DeviceMode.Overlapped)
            {
                var security = new NativeMethods.SECURITY_ATTRIBUTES();
                var overlapped = new NativeOverlapped();

                var overlapTimeout = timeout <= 0 ? NativeMethods.WAIT_INFINITE : timeout;

                security.lpSecurityDescriptor = IntPtr.Zero;
                security.bInheritHandle = true;
                security.nLength = Marshal.SizeOf(security);

                overlapped.OffsetLow = 0;
                overlapped.OffsetHigh = 0;
                overlapped.EventHandle = NativeMethods.CreateEvent(ref security, Convert.ToInt32(false), Convert.ToInt32(true), "");

                try
                {
                    NativeMethods.WriteFile(WriteHandle, buffer, (uint)buffer.Length, out bytesWritten, ref overlapped);
                }
                catch { return false; }

                var result = NativeMethods.WaitForSingleObject(overlapped.EventHandle, overlapTimeout);

                switch (result)
                {
                    case NativeMethods.WAIT_OBJECT_0:
                        return true;
                    case NativeMethods.WAIT_TIMEOUT:
                        return false;
                    case NativeMethods.WAIT_FAILED:
                        return false;
                    default:
                        return false;
                }
            }
            else
            {
                try
                {
                    var overlapped = new NativeOverlapped();
                    return NativeMethods.WriteFile(WriteHandle, buffer, (uint)buffer.Length, out bytesWritten, ref overlapped);
                }
                catch { return false; }
            }
        }

        private HidDeviceData ReadData(int timeout)
        {
            var buffer = new byte[] { };
            var status = HidDeviceData.ReadStatus.NoDataRead;

            if (_deviceCapabilities.InputReportByteLength > 0)
            {
                uint bytesRead = 0;

                buffer = CreateInputBuffer();

                if (_deviceReadMode == DeviceMode.Overlapped)
                {
                    var security = new NativeMethods.SECURITY_ATTRIBUTES();
                    var overlapped = new NativeOverlapped();
                    var overlapTimeout = timeout <= 0 ? NativeMethods.WAIT_INFINITE : timeout;

                    security.lpSecurityDescriptor = IntPtr.Zero;
                    security.bInheritHandle = true;
                    security.nLength = Marshal.SizeOf(security);

                    overlapped.OffsetLow = 0;
                    overlapped.OffsetHigh = 0;
                    overlapped.EventHandle = NativeMethods.CreateEvent(ref security, Convert.ToInt32(false), Convert.ToInt32(true), string.Empty);

                    try
                    {
                        NativeMethods.ReadFile(ReadHandle, buffer, (uint)buffer.Length, out bytesRead, ref overlapped);

                        var result = NativeMethods.WaitForSingleObject(overlapped.EventHandle, overlapTimeout);

                        switch (result)
                        {
                            case NativeMethods.WAIT_OBJECT_0: status = HidDeviceData.ReadStatus.Success; break;
                            case NativeMethods.WAIT_TIMEOUT: 
                                status = HidDeviceData.ReadStatus.WaitTimedOut;
                                buffer = new byte[] {};
                                break;
                            case NativeMethods.WAIT_FAILED:
                                status = HidDeviceData.ReadStatus.WaitFail;
                                buffer = new byte[] { };
                                break;
                            default:
                                status = HidDeviceData.ReadStatus.NoDataRead;
                                buffer = new byte[] { };
                                break;
                        }
                    }
                    catch { status = HidDeviceData.ReadStatus.ReadError; }
                    finally { CloseDeviceIO(overlapped.EventHandle); }
                }
                else
                {
                    try
                    {
                        var overlapped = new NativeOverlapped();
                        
                        NativeMethods.ReadFile(ReadHandle, buffer, (uint)buffer.Length, out bytesRead, ref overlapped);
                        status = HidDeviceData.ReadStatus.Success;
                    }
                    catch { status = HidDeviceData.ReadStatus.ReadError; }
                }
            }
            return new HidDeviceData(buffer, status);
        }

        private static IntPtr OpenDeviceIO(string devicePath, uint deviceAccess)
        {
            return OpenDeviceIO(devicePath, DeviceMode.NonOverlapped, deviceAccess);
        }

        private static IntPtr OpenDeviceIO(string devicePath, DeviceMode deviceMode, uint deviceAccess)
        {
            var security = new NativeMethods.SECURITY_ATTRIBUTES();
            var flags = 0;

            if (deviceMode == DeviceMode.Overlapped) flags = NativeMethods.FILE_FLAG_OVERLAPPED;

            security.lpSecurityDescriptor = IntPtr.Zero;
            security.bInheritHandle = true;
            security.nLength = Marshal.SizeOf(security);

            return NativeMethods.CreateFile(devicePath, deviceAccess, NativeMethods.FILE_SHARE_READ | NativeMethods.FILE_SHARE_WRITE, ref security, NativeMethods.OPEN_EXISTING, flags, 0);
        }

        private static void CloseDeviceIO(IntPtr handle)
        {
            NativeMethods.CloseHandle(handle);
        }

        private void DeviceEventMonitorInserted()
        {
            if (IsOpen) OpenDevice();
            if (Inserted != null) Inserted();
        }

        private void DeviceEventMonitorRemoved()
        {
            if (IsOpen) CloseDevice();
            if (Removed != null) Removed();
        }

        public void Dispose()
        {
            if (MonitorDeviceEvents) MonitorDeviceEvents = false;
            if (IsOpen) CloseDevice();
        }
    }
}
