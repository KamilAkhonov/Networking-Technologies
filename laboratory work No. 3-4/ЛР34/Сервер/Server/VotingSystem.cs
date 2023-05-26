using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Server
{
    internal class VotingSystem
    {
        public VotingSystem()
        {

        }
        public string Close_Void()
        {
            try
            {
                string filePath = "candidates.csv";
                string fileData = File.ReadAllText(filePath);
                return fileData;
            }
            catch (Exception ex)
            {
                return "Error at the end of voting";
            }
        }

        public bool Voit(string account_name)
        {
            try
            {
                string filePath = "candidates.csv";
                string[] lines = File.ReadAllLines(filePath);

                string lastNameToFind = account_name; // Заданная фамилия

                for (int i = 0; i < lines.Length; i++)
                {
                    string[] columns = lines[i].Split(','); // Разделение строк на столбцы

                    if (columns.Length != 2)
                    {
                        Console.WriteLine($"Ошибка в строке {i + 1}: неверное количество столбцов.");
                        continue;
                    }

                    string lastName = columns[0].Trim(); // Фамилия (1-й столбец)
                    int votes;

                    if (int.TryParse(columns[1], out votes)) // Число голосов (2-й столбец)
                    {
                        if (lastName.Equals(lastNameToFind, StringComparison.OrdinalIgnoreCase))
                        {
                            votes++; // Увеличение числа голосов
                            columns[1] = votes.ToString(); // Обновление значения в массиве столбцов
                            lines[i] = string.Join(",", columns); // Объединение столбцов в строку с разделителем ","
                        }
                    }
                    else
                    {
                        Console.WriteLine($"Ошибка в строке {i + 1}: неверный формат числа.");
                        continue;
                    }
                }

                // Запись обновленных данных в файл
                File.WriteAllLines(filePath, lines);
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }
    }
}
