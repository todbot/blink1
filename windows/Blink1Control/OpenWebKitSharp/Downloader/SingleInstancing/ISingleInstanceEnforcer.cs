using System;

namespace MyDownloader.App.SingleInstancing
{
    /// <summary>
    /// Provides methods which a single instance application can use to in order to be respond to any new instance of it.
    /// </summary>
    public interface ISingleInstanceEnforcer
    {
        /// <summary>
        /// Handles messages received from a new instance of the application.
        /// </summary>
        /// <param name="e">The EventArgs object holding information about the event.</param>
        void OnMessageReceived(MessageEventArgs e);
        /// <summary>
        /// Handles a creation of a new instance of the application.
        /// </summary>
        /// <param name="e">The EventArgs object holding information about the event.</param>
        void OnNewInstanceCreated(EventArgs e);
    }
}