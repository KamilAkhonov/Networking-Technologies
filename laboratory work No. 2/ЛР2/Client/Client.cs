using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Client
{
    class Client
    {
        Socket handler;

        TextBox ResponseBox;
        public Client(TextBox box)
        {
            ResponseBox = box;
        }
        public void Connect(string IP, int port)
        {
            handler = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                handler.Connect(IP, port);
            }
            catch (SocketException e)
            {
                MessageBox.Show("Не удалось подключиться к серверу");
            }
        }

        public void Disconnect()
        {
            handler.Close();
        }
        public enum Command
        {
            GetList,
            ChangePrice
        }
        public void GetList()
        {
            try
            {
                byte[] buffer = BitConverter.GetBytes((int)Command.GetList);
                handler.Send(buffer);

                string list = RecvString(handler);

                ResponseBox.Text = list;
            }
            catch (SocketException)
            {
                MessageBox.Show("Не подключен к серверу");
            }
        }
        public void ChangePrice(string product_name, int price)
        {
            try
            {
                byte[] buffer = BitConverter.GetBytes((int)Command.ChangePrice);
                handler.Send(buffer);

                SendString(product_name);
                SendString(price.ToString());
            }
            catch (SocketException)
            {
                MessageBox.Show("Не подключен к серверу");
            }
        }

        private string RecvString(Socket handler)
        {
            byte[] buffer = new byte[sizeof(int)];
            handler.Receive(buffer);
            buffer = new byte[BitConverter.ToInt32(buffer)];
            handler.Receive(buffer);

            return Encoding.UTF8.GetString(buffer);
        }
        private void SendString(string str)
        {
            handler.Send(BitConverter.GetBytes(str.Length));
            handler.Send(Encoding.UTF8.GetBytes(str));
        }
    }
}
