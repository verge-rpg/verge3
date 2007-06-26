using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace winmaped2
{
	class ThrottleBuffer
	{
		int delay;
		Thread thread;
		EventHandler callback;
		public ThrottleBuffer(int delay, EventHandler callback)
		{
			this.delay = delay;
			this.callback = callback;
			thread = new Thread(run);
			thread.IsBackground = true;
			thread.Start();
		}
		EventWaitHandle synch = new EventWaitHandle(false, EventResetMode.AutoReset);

		public void signal()
		{
			lock (this)
			{
				synch.Set();
			}
		}

		void run()
		{
			for (; ; )
			{
				synch.WaitOne();
				callback(this, null);
				Thread.Sleep(delay);
			}
		}
	}
}
