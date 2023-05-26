namespace Server
{
    public partial class ServerForm : Form
    {
        public ServerForm()
        {
            InitializeComponent();
            server = new Server(LogBox);
        }

        Server server;
        private void �����ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Task start = new Task(()=>{ server.Start(5000); });

            start.Start();
        }

        private void ����ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            server.Stop();
        }
    }
}