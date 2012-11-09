using System;

namespace MyDownloader.App.SingleInstancing
{
    /// <summary>
    /// Provides a proxy to communicate with the first instance of the application.
    /// </summary>
    internal class SingleInstanceProxy : MarshalByRefObject
    {
        #region Member Variables

        private ISingleInstanceEnforcer enforcer;

        #endregion

        #region Construction / Destruction

        /// <summary>
        /// Instantiates a new SingleInstanceProxy object.
        /// </summary>
        /// <param name="enforcer">The enforcer (first instance of the application) which will receive messages from the new instances of the application.</param>
        /// <exception cref="System.ArgumentNullException">enforcer is null.</exception>
        public SingleInstanceProxy(ISingleInstanceEnforcer enforcer)
        {
            if (enforcer == null)
                throw new ArgumentNullException("enforcer", "enforcer cannot be null.");

            this.enforcer = enforcer;
        }

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets the enforcer (first instance of the application) which will receive messages from the new instances of the application.
        /// </summary>
        public ISingleInstanceEnforcer Enforcer
        {
            get
            {
                return enforcer;
            }
        }

        #endregion

        ///
        /// Override and return NULL to prevent windows from doing
        /// garbage collection on the proxy object.
        ///
        public override object InitializeLifetimeService()
        {
            return null;
        }
    }
}