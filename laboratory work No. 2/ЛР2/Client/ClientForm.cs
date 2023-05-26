namespace Client
{
    public partial class ClientForm : Form
    {
        public ClientForm()
        {
            InitializeComponent();
            client = new Client(ResponseBox);
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

        private void button1_Click(object sender, EventArgs e)
        {
            client.GetList();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                client.ChangePrice(ProductNameBox.Text, int.Parse(PriceBox.Text));
            }
            catch (FormatException)
            {
                MessageBox.Show("Неправильно указана цена");
            }
        }
    }
}