using System;
using System.Collections;
using System.Windows.Forms;
using System.Drawing;

namespace winmaped2
{
	public class ColorPicker : Panel
	{
		public ColorInfo colorInfo = new ColorInfo();
		ColorSlider cs_r,cs_g,cs_b;
		TextBox tx_r, tx_g, tx_b;
		public Color SelectedColor { get { return colorInfo.ToColor(); } }
		public ColorPicker() : base()
		{
			cs_r = new ColorSlider();
			cs_g = new ColorSlider();
			cs_b = new ColorSlider();

			tx_r = new TextBox();
			tx_g = new TextBox();
			tx_b = new TextBox();

			tx_r.Text = "0";
			tx_g.Text = "0";
			tx_b.Text = "0";

			tx_r.TextChanged += new EventHandler(txChanged);
			tx_g.TextChanged += new EventHandler(txChanged);
			tx_b.TextChanged += new EventHandler(txChanged);

			tx_r.KeyPress += new KeyPressEventHandler(StopNonIntKeys);
			tx_g.KeyPress += new KeyPressEventHandler(StopNonIntKeys);
			tx_b.KeyPress += new KeyPressEventHandler(StopNonIntKeys);

			cs_r.ParentColor = colorInfo;
			cs_g.ParentColor = colorInfo;
			cs_b.ParentColor = colorInfo;

			cs_r.PrimaryColorChannel = ColorSlider.ColorChannel.Red;
			cs_g.PrimaryColorChannel = ColorSlider.ColorChannel.Green;
			cs_b.PrimaryColorChannel = ColorSlider.ColorChannel.Blue;

			cs_r.Location = new System.Drawing.Point(0,0);
			cs_g.Location = new System.Drawing.Point(0,32);
			cs_b.Location = new System.Drawing.Point(0,64);

			tx_r.Location = new System.Drawing.Point(136,0);
			tx_g.Location = new System.Drawing.Point(136,32);
			tx_b.Location = new System.Drawing.Point(136,64);

			cs_r.Size = new System.Drawing.Size(128,20);
			cs_g.Size = new System.Drawing.Size(128,20);
			cs_b.Size = new System.Drawing.Size(128,20);

			tx_r.Size = new System.Drawing.Size(32,20);
			tx_g.Size = new System.Drawing.Size(32,20);
			tx_b.Size = new System.Drawing.Size(32,20);

			Controls.Add(cs_r);
			Controls.Add(cs_g);
			Controls.Add(cs_b);
			Controls.Add(tx_r);
			Controls.Add(tx_g);
			Controls.Add(tx_b);

			colorInfo.ColorChanged += new ColorInfo.CEvent(colorInfo_ColorChanged);


		}
		private bool _ignoretx=false;

		private bool _ignoreci=false;
		private void colorInfo_ColorChanged()
		{
			Invalidate(true);
			if(_ignoreci) return;

			_ignoretx=true;
			tx_r.Text = colorInfo.R.ToString();
			tx_g.Text = colorInfo.G.ToString();
			tx_b.Text = colorInfo.B.ToString();
			_ignoretx=false;
			
		}
		private void StopNonIntKeys(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			if ( (e.KeyChar < '0' || e.KeyChar > '9') && e.KeyChar != 8)
				e.Handled = true;
		}

		private int parse(string s)
		{
			int cnt = 0;
			while(cnt<s.Length&&char.IsDigit(s[cnt]))
				cnt++;
			if(cnt==0) return 0;
			s = s.Substring(0, cnt);
			int i=0;
			int val=0;
			for(; cnt>0;cnt--)
				val += (int)char.GetNumericValue(s[cnt-1]) * (int)Math.Pow(10,i++);
			return val;
		}


		private void txChanged(object sender, EventArgs e)
		{
			if(_ignoretx)
			{
				return;
			}
			int R=0,G=0,B=0;
			R = parse(tx_r.Text);
			G = parse(tx_g.Text);
			B = parse(tx_b.Text);
			if(R>255||R<0) R = 0;
			if(G>255||G<0) G = 0;
			if(B>255||B<0) B = 0;
			_ignoreci = true;
			colorInfo.R = R;
			colorInfo.G = G;
			colorInfo.B = B;
			_ignoreci = false;
		}
	}
}
