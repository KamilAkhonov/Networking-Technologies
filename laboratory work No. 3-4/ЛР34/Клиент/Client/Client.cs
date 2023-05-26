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

        TcpClient client;

        TextBox textBox1;
        TextBox textBox2;

        public Client(TextBox box, TextBox box2)
        {
            textBox1 = box;
            textBox2 = box2;
        }
        public void Connect(string IP, int port)
        {
            try
            {
                client = new TcpClient(IP, port);
                MessageBox.Show("удалось подключиться к серверу");
            }
            catch (SocketException e)
            {
                MessageBox.Show("Не удалось подключиться к серверу");
            }
        }

        public void Disconnect()
        {
            textBox1.Text = "";
            textBox2.Text = "";
            client.Close();
        }

        public enum Command
        {
            Voit,
            CloseTheVoid
        }

        public void Voit(string candidateName)
        {
            try
            {
                // Отправляем на сервер имя кандидата
                var stream = client.GetStream();
                byte[] buffer = BitConverter.GetBytes((int)Command.Voit);
                stream.Write(buffer, 0, buffer.Length);


                stream = client.GetStream();
                byte[] data = Encoding.UTF8.GetBytes(candidateName);
                stream.Write(data, 0, data.Length);


                // Читаем ответ от сервера
                data = new byte[256];
                int bytes = stream.Read(data, 0, data.Length);
                string response = Encoding.UTF8.GetString(data, 0, bytes);

                // Если получили сообщение "успешно", выводим его в текстовое поле
                if (response == "успешно")
                {
                    textBox2.AppendText("Ваш голос за кандидата " + candidateName + " засчитан.\r\n");
                }
                else
                {
                    if (response == "Вы уже голосовали и/или голосование было завершено")
                    {
                        textBox2.Text = response;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public void CloseTheVoid()
        {
            try
            {
                // Отправляем на сервер команду "CloseTheVoid"
                var stream = client.GetStream();
                byte[] buffer = BitConverter.GetBytes((int)Command.CloseTheVoid);
                stream.Write(buffer, 0, buffer.Length);

                //Читаем ответ от сервера
                byte[] data = Encoding.UTF8.GetBytes("CloseTheVoid");
                data = new byte[1024];
                int bytes = stream.Read(data, 0, data.Length);
                string response = Encoding.UTF8.GetString(data, 0, bytes);

                //Выводим список кандидатов с количеством голосов в текстовое поле
                textBox2.AppendText(response + "\r\n");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

    }
}
