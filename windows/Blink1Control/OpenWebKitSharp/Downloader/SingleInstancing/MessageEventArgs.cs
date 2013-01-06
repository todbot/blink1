using System;

namespace MyDownloader.App.SingleInstancing
{
    /// <summary>
    /// Provides data for the SingleInstancing.ISingleInstanceEnforcer.OnMessageReceived method.
    /// </summary>
    [Serializable()]
    public class MessageEventArgs : EventArgs
    {
        #region Member Variables

        private object message;

        #endregion

        #region Construction / Destruction

        /// <summary>
        /// Instantiates a new MessageEventArgs object.
        /// </summary>
        /// <param name="message">The message to pass to the first running instance of the application.</param>
        /// <exception cref="System.ArgumentNullException">message is null.</exception>
        public MessageEventArgs(object message)
        {
            if (message == null)
                throw new ArgumentNullException("message", "message cannot be null.");

            this.message = message;
        }

        #endregion

        #region Properties

        /// <summary>
        /// Gets the message sent to the first instance of the application.
        /// </summary>
        public object Message
        {
            get
            {
                return message;
            }
        }

        #endregion
    }
}