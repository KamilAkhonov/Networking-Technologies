using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Client
{
    public partial class Form1 : Form
    {


        public Form1()
        {
            InitializeComponent();
            client = new Client(textBox1,textBox2);
        }

        Client client;

        private void подключитьсяToolStripMenuItem_Click(object sender, EventArgs e)
        {
            client.Connect("127.0.0.1", 5000);
        }

        private void отключитьсяToolStripMenuItem_Click(object sender, EventArgs e)
        {
            client.Disconnect();
        }


        private void Form1_Load(object sender, EventArgs e)
        {


        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            client.Voit(textBox1.Text);
        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            client.CloseTheVoid();
        }
    }
}
