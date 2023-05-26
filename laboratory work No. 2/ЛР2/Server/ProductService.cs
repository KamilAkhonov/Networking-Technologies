namespace Server
{
    internal class ProductService
    {
        public ProductService()
        {
            if (!Directory.Exists("database"))
                Directory.CreateDirectory("database");
            if (!File.Exists("database\\list.txt"))
                File.Create("database\\list.txt");

        }

        public string GetProducts()
        {
            using var filestream = new StreamReader("database\\list.txt");
            return filestream.ReadToEnd();
        }

        public bool ChangePrice(string product_name, int price)
        {
            try
            {
                string result = "";
                using (var filestream = new StreamReader("database\\list.txt"))
                {
                    string? line = filestream.ReadLine();
                    while (line != null)
                    {
                        if (line.Split(new[] { ' ' })[0] == product_name)
                        {
                            result += (line.Split(new[] { ' ' })[0] + " " + price.ToString() + Environment.NewLine);
                        }
                        else
                        {
                            result += line + Environment.NewLine;
                        }
                        line = filestream.ReadLine();
                    }
                }

                using (var filestream = new StreamWriter("database\\list.txt"))
                {
                    filestream.Write(result);
                }
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

    }
}
