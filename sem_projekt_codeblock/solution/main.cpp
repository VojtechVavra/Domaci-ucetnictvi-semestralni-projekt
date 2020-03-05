/**
 * @file main.cpp
 * @brief Hlavni soubor. Obsahuje cely program.
 * @author Vojtech Vavra
 * @mainpage VAV0205 - Domaci ucetnictvi
 * @date 10. 12. 2018
 */

#include <iostream>		// std::fixed
#include <fstream>		// file stream for read and write
#include <sstream>		// istringstream
#include <string>		// std::string, std::stoi
#include <vector>
#include <algorithm>	// std::find
#include <cmath>
//#include <ctime>
#include <time.h>
#include <iomanip>      // std::setprecision
#include <stdexcept>	// exception handling mechanism (try - catch block)
#include <list>


#define DELIMITER ','	/*!< csv delimiter setup */

using namespace std;

/**
 * @brief struktura ucetnich dat, zde se ukladaji jednotlive polozky
 */
struct UcetniData
{
	int ID;                 /**< Unikatni ID zaznamu. */
	string prijemVydaj;		/**< Prijem, nebo vydaj. */
	string kategorie;       /**< Kategorie zaznamu. */
	double castka;			/**< Maximalni castka 999,999,999 Kc. */
	string day;             /**< Den zaznamu. */
	string month;           /**< Mesic zaznamu. */
	string year;            /**< Rok zaznamu. */
	//string datum;			// korektni format casu DD/MM/YYYY
};

/** @struct Months
 *  @brief Struktura obsahujici pocet dnu v danem mesici.
 *  @param Months.daysOfMonth   Pocet dnu mesice
 *  @param Months.nazvyMesicu   Nazev mesice
 */
struct Months
{
	int daysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	/**<  Unor 28 - 29 dni v zavyslosti na prestupnem roku */
	string nazvyMesicu[12] = { "Leden", "Unor", "Brezen", "Duben", "Kveten", "Cerven", "Cervenec", "Srpen", "Zari", "Rijen", "Listopad", "Prosinec" }; /**<  nazvy mesicu */

	public:
		int NumOFDays(int month, int year)
		{
			if (month == 2)
			{
				if (year % 4 == 0 || year % 400 == 0)	// podminka prestupneho roku
					return daysOfMonth[month-1] + 1;	/**<  return 29 */
				else
					return daysOfMonth[month-1];		/**< return 28 */
			}
			else
				return daysOfMonth[month-1];            /**< return normalni pocet dnu v danem mesici */
	}
	// prestupny rok	 {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};						 -----

	/* gregoriánský kalendář, který má přestupný každý čtvrtý rok (rok dělitelný beze zbytku
	   čtyřmi), ale roky dělitelné 100 jsou přestupné jenom tehdy, jsou-li dělitelné také 400.
	*/
};

/** @struct ErrorText
 *  @brief Struktura obsahujici id a info o chybe.
 *  @param ErrorText.id   Id, kde je chyba obsazena
 *  @param ErrorText.info   Info o chybe
 */
struct ErrorText
{
	vector<int> id;         /*!< id ve kterem je chyba */
	vector<string> info;    /*!< info o chybe */
};

void Menu(vector<UcetniData>&, ErrorText&);
void Setup(vector<UcetniData>&, ErrorText&);
string GetDataPath();
string GetOutputHtmlPath();
bool FileExist(string);
vector<UcetniData> loadData(string, ErrorText&);
void printTable(vector<UcetniData>);

double CheckMoney(string, ErrorText&, int);
bool MoneyIsNotOverMaxValue(double);
string SpacedMoneyValue(double);

void AddData(vector<UcetniData>&);
void TimeFormat(string&, unsigned int, ErrorText&, int, vector<UcetniData> &values);
bool TryConvertFromString(string, int = 0);
//void AddErrorToLog(string);
bool IsValidID(string, vector<UcetniData>, ErrorText&);
bool IsIdDuplicated(int, vector<UcetniData>);
string CheckIncomeExpenditure(string);
void PrintErrors(ErrorText&);
void CreateHtml(vector<UcetniData>);

char TIME_DELIMITER = '.';		/*!< '.', '-', ':' */
string MONEY_DELIMITER = ",";	/*!< " ", ",", "." delimeters that user can choose between to show */
string filePath;        /*!< cesta k vstupnimu souboru */
string outputHtmlPath; /*!< cesta k vystupnimu souboru */
const string defaultPath = "..\\vstupnidata\\data.csv";     /*!< zakladni cesta vstupu */
const string defaultOutputHtmlpath = "..\\vystupnidata\\out.html";  /*!< zakladni cesta vystupu */
const string inPathFolder = "..\\vstupnidata\\";  /*!< cesta do slozky se vstupnimy daty */
const string outPathFolder = "..\\vystupnidata\\"; /*!< cesta do slozky s vystupnimi daty */
time_t rawtime = time(nullptr);     /*!< time */

/**
 * @brief Hlavni funkce programu. Vola se z ni Menu.
 * @return 0 pokud se program ukonci uspesne.
 */
int main()
{
	ErrorText errorText;
	vector<UcetniData> ucetniData;
	Menu(ucetniData, errorText);

    return 0;
}

/**
 * @brief Funkce vyvola hlavni menu
 * @param ucetniData vector ucetniData
 * @param errorText vector errorText
 */
void Menu(vector<UcetniData> &ucetniData, ErrorText &errorText)
{
	int neplatnePokusy = 0;
	do
	{
		cout << "Hlavni menu" << endl;
		cout << "1 - Nastaveni" << endl;
		//cout << "2 - Vypsat csv tabulku do konzole" << endl;
		cout << "2 - Nacist data z .csv souboru" << endl;
		cout << "3 - Pridat data do tabulky" << endl;
		cout << "4 - Vytvorit .html soubor s tabulkou souhrnu dat" << endl;
		cout << "5 - Ukoncit program" << endl;

		cout << endl << "Zadejte cislo vami pozadovane akce:" << endl;

		int moznost;
		cin >> moznost;
		if(moznost != 1)
            cout << endl;

		switch (moznost)
		{
		case 1:
			Setup(ucetniData, errorText);
			break;
		case 2:
			if (ucetniData.size() == 0) {
				if (filePath.length() == 0)
					ucetniData = loadData(defaultPath, errorText);
				else
					ucetniData = loadData(filePath, errorText);
			}
			printTable(ucetniData);
			PrintErrors(errorText);
			break;
		case 3:
			if (ucetniData.size() == 0){
				if (filePath.length() == 0)
					ucetniData = loadData(defaultPath, errorText);
				else
					ucetniData = loadData(filePath, errorText);
			}
			AddData(ucetniData);
			break;
		case 4:
			if (ucetniData.size() == 0){
				if (filePath.length() == 0)
					ucetniData = loadData(defaultPath, errorText);
				else
					ucetniData = loadData(filePath, errorText);
			}
			CreateHtml(ucetniData);
			break;
		case 5: exit(EXIT_SUCCESS);
		default:
			if (neplatnePokusy == 9){
				cout << "\nProgram bude ukoncen." << endl << endl;
				neplatnePokusy++;
			}
			else {
				cout << "Zadali jste nespravnou moznost!" << endl;
				cout << "Zkuste to znovu." << endl;
				cout << endl;
				neplatnePokusy++;

				cin.clear(); // clear the failed state of the stream
				cin.ignore(1000000, '\n'); // extract and discard the bad input
				//cin.ignore();
			}
			break;
		}
	} while (neplatnePokusy < 10);
}

/**
 * @brief Funkce podmenu s nastavenim
 * @param ucetniData vector ucetniData
 * @param errorText vector errorText
 */
void Setup(vector<UcetniData> &ucetniData, ErrorText &errorText)
{
	bool back = false;
	do
	{
	    struct tm *timeinfo;
	    time(&rawtime);
        timeinfo = localtime ( &rawtime );

        string cDay = to_string(timeinfo->tm_mday);
        string cMonth = to_string(1 + timeinfo->tm_mon);
        string cYear = to_string(1900 + timeinfo->tm_year);

        if (cDay.length() < 2)
            cDay.insert(0, "0");
        if (cMonth.length() < 2)
            cMonth.insert(0, "0");
        string currentDate = cDay + TIME_DELIMITER + cMonth + TIME_DELIMITER + cYear;

		cout << "---------------------------------------------------------------------------------------" << endl;
		cout << endl;
		cout << "Nastaveni" << endl << endl;
		cout << "Aktualni cesta k souboru:" << endl;
		cout << (filePath.length() == 0 ? defaultPath : filePath) << endl;
		cout << "Aktualni cesta k vystupnimu souboru:" << endl;
		cout << (outputHtmlPath.length() == 0 ? defaultOutputHtmlpath : outputHtmlPath) << endl << endl;

		cout << "Oddelovac casu:   " + currentDate << endl;
		cout << "Oddelovac penez:  10" + MONEY_DELIMITER + "692" + MONEY_DELIMITER + "588" + " Kc" << endl;
		cout << endl;

		cout << "Zadejte cislo akce:" << endl;
		cout << "1 - Zmena cesty k souboru" << endl;
		cout << "2 - Zmena nazvu vystupniho souboru" << endl;
		cout << "3 - Zmena oddelovace casu" << endl;
		cout << "4 - Zmena oddelovace penez" << endl;
		cout << "5 - Navrat do hlavniho menu" << endl;

		int result;
		int d;
		cin.clear();
		cin >> result;

		switch (result)
		{
		case 1:
			filePath = GetDataPath();
			if (filePath == "-1")
				exit(EXIT_SUCCESS);
			if (errorText.id.size() != 0)
			{
				errorText.id.clear();
				errorText.info.clear();
			}
			ucetniData = loadData(filePath, errorText);
			break;
		case 2:
			outputHtmlPath = GetOutputHtmlPath();
			if (outputHtmlPath == "-1")
				outputHtmlPath = defaultOutputHtmlpath;
			break;
		case 3:
			d = 1;
			cout << endl << "Vyberte oddelovac casu:" << endl;
			cout << "1 - 01.12.2018" << endl;
			cout << "2 - 01-12-2018" << endl;
			cout << "3 - 01:12:2018" << endl;
			cin >> d;
			if (cin.fail())
			{
				TIME_DELIMITER = '.';
				cin.clear();
				cin.ignore(1000000, '\n');
			}
			else if (d == 1)
				TIME_DELIMITER = '.';
			else if (d == 2)
				TIME_DELIMITER = '-';
			else if (d == 3)
				TIME_DELIMITER = ':';
			break;
		case 4:
			d = 1;
			cout << endl << "Vyberte oddelovac penez:" << endl;
			cout << "1 - 10,692,588 Kc" << endl;
			cout << "2 - 10 692 588 Kc" << endl;
			cin >> d;
			if (cin.fail())
			{
				MONEY_DELIMITER = ',';
				cin.clear();
				cin.ignore(1000000, '\n');
			}
			else if (d == 1)
				MONEY_DELIMITER = ',';
			else if (d == 2)
				MONEY_DELIMITER = ' ';
			break;
		case 5:
			back = true;
			break;
		default:
			cout << "Nespravna volba!" << endl << endl;
			cin.clear();
			cin.ignore(1000000, '\n');
			back = true;
			break;
		}

	} while (!back);

	cout << "---------------------------------------------------------------------------------------" << endl << endl;
}

/**
 * @brief Funkce pro ziskani cesty k souboru
 * @return string uzivatelske cesty k souboru
 */
string GetDataPath()
{
	string path;
	bool fileExist = false;
	//bool exit = false;
	short attempt = 1;
	cout << "Zadejte nazev .csv souboru, nebo jeho cestu:" << endl;

	while(attempt < 10)
	{
		cin.clear();

		//getline(cin, path);
		cin >> path;

        fileExist = FileExist(path);
		if (fileExist)
		{
            cout << endl;
			return(path);
		}
		else if(FileExist("..\\vstupnidata\\" + path))
        {
            cout << endl;
            fileExist = true;
			return(inPathFolder + path);
        }
		else
        {
            fileExist = FileExist(inPathFolder + path + ".csv");
            if (fileExist)
            {
                cout << endl;
                return(inPathFolder + path + ".csv");
            }
            else
            {
                fileExist = FileExist(inPathFolder + path + ".txt");
                if (fileExist)
                {
                    cout << endl;
                    return(inPathFolder + path + ".txt");
                }
                else if (attempt % 2 == 0)
                {
                    cout << "Zadali jste neplatnou cestu, nebo soubor neexistuje." << endl;
                    cout << "Prejete si ukoncit zadavani cesty? [A/N]" << endl;
                    string answer;
                    cin >> answer;
                    if (answer == "a" || answer == "A")
                        attempt = 10;
                    else
                    {
                        attempt++;
                        if (attempt < 10)
                            cout << "Zadejte cestu k souboru:" << endl;
                    }
                }
                else
                {
                    if (attempt < 9)
                        cout << "Zadali jste neplatnou cestu, nebo soubor neexistuje. Zkuste to znovu:" << endl;
                    else
                        cout << "\nZadavali jste prilis mnoho neplatnych cest k souboru." << endl;
                    attempt++;
                }
            }
        }
	}
	cout << "Pouzije se zakladni cesta k souboru." << endl;
	if (!FileExist(defaultPath))
	{
		cout << "\nZakladni cesta k souboru neexistuje!\nProgram bude ukoncen." << endl;
		return("-1");
	}
	else
	{
		cout << endl;
		return(defaultPath);
	}
}

/**
 * @brief Funkce pro ziskani vystupni cesty
 * @return string s cestou k vystupnimu souboru
 */
string GetOutputHtmlPath()
{
	string htmlPath;

	cout << "Zadejte nazev .html souboru:" << endl;
	cin.clear();
	cin >> htmlPath;

    for (int x = 0; x < htmlPath.length(); x++)
    {
        if(!isalnum(htmlPath[x]))
        {
            cout << "Neplatny nazev! Pouzije se zakladni nazev \"out.html\"" << endl;
            return("out.html");
        }
    }
	cout << endl;
	return(outPathFolder + htmlPath + ".html");
}

/**
 * @brief Funkce pro kontrolu souboru, jestli existuje
 * @param path cesta k udajnemu souboru
 * @return true pokud soubor existuje, false pokud soubor neexistuje
 */
bool FileExist (string path)
{
	ifstream fin(path);
	return !fin.fail();
}

/**
 * @brief Funkce pro nacteni dat z .csv souboru
 * @param pathToCSV cesta k souboru
 * @param errorText struktura, pro ukladani chyb ze vstupu
 * @return vector UcetnichDat
 */
vector<UcetniData> loadData(string pathToCSV, ErrorText &errorText)
{
	ifstream inputData(pathToCSV);

	if (inputData.fail()) {
		// file could not be opened
		cout << "Soubor nenalezen!" << endl;
		cout << "Program se ukonci." << endl;
		//cin.get();
		exit(EXIT_FAILURE);
	}

	vector<UcetniData> values;
	//vector<UcetniData> values = { UcetniData() };
	//vector< vector<string> > values;  // vector of vector's strings
	// cin.ignore();  is great for removing the annoying new lines stored at the end of the stream

	if (inputData.is_open())
	{
		string line;
		vector<string> tmp;
		int count;
		unsigned int overallRows = 0;

		//go through every line
		while (getline(inputData, line))
		{
			count = 0;
			if (line.length() == 0)
				continue;

			int delimCount = 0;
			for (unsigned int n = 0; n < line.length(); n++)
			{
				if (line[n] == DELIMITER && n == 0)
				{
					line.insert(0, " ");
					//delimCount++;
				}
				if (line[n] == DELIMITER)
				{
					delimCount++;
					if (n != line.length() - 1 && line[n + 1] == DELIMITER)
						line.insert(n + 1, " ");
				}
			}
			if (line[line.length() - 1] == DELIMITER)	// pokud je prazdne datum, dopln mezeru
				line += " ";
			if (delimCount < 4)					// detekce, jestli nejsou na radku 4 oddelovace poli, tak preskoci radek
				continue;
			/*while (delimCount < 4) {
				line += ", ";
				delimCount++;
			}*/

			istringstream s(line);
			string field;
			values.push_back(UcetniData());		// add row to 'values'

			while (getline(s, field, DELIMITER))
			{
				//Push back new subject created with default constructor.
				//Vector now has 1 element @ index 0, so modify it.
				//Add a new element if you want another: again like: values.push_back(UcetniData());
				if (count == 3) // if (count == 0 || count == 3)
				{
					if (!TryConvertFromString(field, count))
					{
						//values.pop_back();
						//overallRows--;
						//break;
					}
				}
				if (count == 0)
				{
					if (!TryConvertFromString(field, count))
					{
						field = "-1";
					}
				}

				switch (count)
				{
				case 0: values[overallRows].ID = IsValidID(field, values, errorText) ? stoi(field) : -1; break;	// stoi(str) convert str to int
				case 1: values[overallRows].prijemVydaj = CheckIncomeExpenditure(field); break;
				case 2: values[overallRows].kategorie = field; break;
				case 3: values[overallRows].castka = CheckMoney(field, errorText, values[overallRows].ID); break;
				case 4:
					TimeFormat(field, overallRows, errorText, values[overallRows].ID, values);	// check for correct time
					break;
				default:
					break;
				}
				if (count == 4)
					count = 0;
				else
					count++;
			}
			overallRows += 1;
		}
	}
	return values;
}

/**
 * @brief Funkce vypise do konzole, do tabulky data z .csv souboru
 * @param val vector ucetnich dat
 */
void printTable(vector<UcetniData> val)
{
	printf(" ______ ________ _________________________ _____________ ____________\n");
	printf("|%4s  |  %-5s | %-6s %-16s |%-1s%-12s|   %-9s|\n", "ID", "Typ", " ", "Kategorie", " ", "Castka [Kc]", "Datum");
	printf("|======+========+=========================+=============+============|\n");
	for (unsigned int i = 0; i < val.size(); i++)
	{
		//prinf c-style
		//If, for some reason, you need to extract the C-style string, you can use the c_str()
		//method of std::string to get a const char * that is null-terminated. Use it like: myString.c_str()
		string date = val[i].day + TIME_DELIMITER + val[i].month + TIME_DELIMITER + val[i].year;
		printf("|%5i | %-6s | %-23.23s | %11.11s | %-10s |\n", val[i].ID, val[i].prijemVydaj.c_str(), val[i].kategorie.c_str(), SpacedMoneyValue(val[i].castka).c_str(), date.c_str());
	}
	printf("|______|________|_________________________|_____________|____________|\n\n");
}

/**
 * @brief Funkce kontroluje zadanou castku, jestli je cislo a jestli neprekrocila limit 999,999,999.5 Kc, nebo neni zaporna
 * @param money zadana castka ve stringu
 * @param errorText struktura pro ukladani chyb
 * @param id pro zaznam chyby
 * @return vraci zadanou castku, jestli odpovida podminkam, -1 vraci, pokud zadana castka neni cislo, -2 vraci jestli je moc velka castka, nebo zaporna
 */
double CheckMoney(string money, ErrorText &errorText, int id)
{
	bool isNotMax;
	double doubleMoney;

	if (TryConvertFromString(money))
	{
		doubleMoney = stod(money);
		isNotMax = MoneyIsNotOverMaxValue(doubleMoney);

		if (isNotMax)
		{
			// money value is correct
			return doubleMoney;
		}
		else
		{
			// money value is bigger than 999,999,999
			errorText.id.push_back(id);
			errorText.info.push_back("Penezni castka presahla maxima 999,999,999 Kc, nebo je zaporna.");
			return -2;
		}
	}
	else
	{
		// money is not a number
		errorText.id.push_back(id);
		errorText.info.push_back("Zadana penezni castka neni cislo.");
		return -1;
	}
}

/**
 * @brief Funkce pro kontrolu velikosti penezni castky
 * @param money zadana castka
 * @return true, pokud castka neprekrocila danou velikost, false pokud prekrocila 999999999.555
 */
bool MoneyIsNotOverMaxValue(double money)
{
	if (money >= 999999999.555 || money < 0.0)
		return false;
	else
		return true;
}

/**
 * @brief Funkce pro prevod penezni castky na formatovany vzhled s oddelovanim po trech cislech
 * @param castka
 * @return string naformatovane penezni castky, nebo vrati "x", pokud je castka -1, nebo -2 ( -1 a -2 znaci nespravny vstup diky kontrole ve funkci CheckMoney)
 */
string SpacedMoneyValue(double castka)
{
	int roundedToInt = (int)round(castka);
	string money = to_string(roundedToInt);
	string spacedMoney = "";
	short count = 0;

	if (castka == -1)
	{
		// Money value is not a number
		// "Neni cislo"
		return "x";
	}
	else if (castka == -2)
	{
		// Money value is too big number (max is: 999,999,999)
		// "Prilis velka castka"
		return "x";
	}

	for (int i = money.length() - 1; i >= 0; i--)
	{
		if (count % 3 == 0 && count != 0) {
			spacedMoney.insert(0, MONEY_DELIMITER); // insert(pos, string) pouzitelne pro vkladani stringu do stringu na danou pozici
		}
		spacedMoney.insert(0, 1, money[i]);			// insert(pos, kolikrat to tam chceme vlozit, char) vkladani znaku typu char do stringu na danou pozici
		count++;
	}
	return spacedMoney;
}

/**
 * @brief Funkce pro pridani novych dat zadavanych uzivatelem z klavesnice
 * @param ucetniData kde se ulozi data k ostatnim datum nactenym z csv
 */
void AddData(vector<UcetniData> &ucetniData)
{
	bool canAdd = true;
		ucetniData.push_back(UcetniData());

		string date;
		unsigned int lengthData = ucetniData.size() - 1;
		int lastId = ucetniData[lengthData - 1].ID;
		char category[24];

		while (IsIdDuplicated(lastId, ucetniData))
		{
			if (lastId < -1)
			{
				lastId = 0;
				lastId++;
			}
			else if (lastId > 9999)
			{
				canAdd = false;
				break;
			}
			else
				lastId++;
		}

		if (!canAdd)
		{
			int idTmp = 0;
			while (IsIdDuplicated(idTmp, ucetniData))
			{
				if (idTmp > 9999)
				{
					canAdd = false;
					break;
				}
				else
					idTmp++;
			}
		}

		ucetniData[lengthData].ID = lastId;

		if (canAdd)
		{
			cout << "Zadejte cislo akce:" << endl;
			cout << "1 - prijem\n2 - vydaj" << endl;
			int num;
			if (cin >> num)
			{
				if (num == 1)
					ucetniData[lengthData].prijemVydaj = "prijem";
				else if (num == 2)
					ucetniData[lengthData].prijemVydaj = "vydaj";
				else
				{
					cout << "Neplatny vstup." << endl << endl;
					canAdd = false;
				}
			}
			else {
				cout << "Neplatny vstup." << endl << endl;
				canAdd = false;
				//break;
			}

			if (canAdd)
			{
				cout << "Zadejte kratky popis max 23 znaku (nepouzivejte znak \",\"):" << endl;
				cin.ignore();
				cin.getline(category, 23);
				ucetniData[lengthData].kategorie = string(category);
				//cin.ignore(INT_MAX);

				cout << "Zadejte castku v Kc:" << endl;
				double money;
				if (cin >> money)
				{
					if (MoneyIsNotOverMaxValue(money))
					{
						ucetniData[lengthData].castka = money;
					}
					else
					{
						cout << "Zadali jste prilis velkou, nebo zapornou castku!" << endl;
						canAdd = false;
						//break;
					}

				}
				else
				{
					cout << "Zadali jste neplatnou castku penez!" << endl;
					canAdd = false;
					//break;
				}
			}
			if (canAdd)
			{
				//struct tm timeinfo;
				//localtime_s(&timeinfo, &t);
				//errno_t error = localtime_s(&timeinfo, &t);

				//cout << asctime_s(localtime(&t)) << '\n';
				//tm *ltm = localtime(&now);
				// print various components of tm structure.
				//cout << "Year" << 1970 + ltm->tm_year << endl;
				//cout << "Month: " << 1 + ltm->tm_mon << endl;
				//cout << "Day: " << ltm->tm_mday << endl;

                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime ( &rawtime );

                string cDay = to_string(timeinfo->tm_mday);
                string cMonth = to_string(1 + timeinfo->tm_mon);
                string cYear = to_string(1900 + timeinfo->tm_year);

                if (cDay.length() < 2)
                    cDay.insert(0, "0");
                if (cMonth.length() < 2)
                    cMonth.insert(0, "0");
                date = cDay + TIME_DELIMITER + cMonth + TIME_DELIMITER + cYear;

                //
				//string cDay = to_string((&timeinfo)->tm_mday);
				//string cMonth = to_string(1 + (&timeinfo)->tm_mon);
				//string cYear = to_string(1900 + (&timeinfo)->tm_year);

				//if (cDay.length() < 2)
				//	cDay.insert(0, "0");
				//if (cMonth.length() < 2)
				//	cMonth.insert(0, "0");

				//date = cDay + TIME_DELIMITER + cMonth + TIME_DELIMITER + cYear;
				ucetniData[lengthData].day = cDay;
				ucetniData[lengthData].month = cMonth;
				ucetniData[lengthData].year = cYear;
				//string date = to_string((&timeinfo)->tm_mday) + TIME_DELIMITER + to_string(1 + (&timeinfo)->tm_mon) + TIME_DELIMITER + to_string( 1900 + (&timeinfo)->tm_year);
			}
			if (canAdd)
			{
				cout << endl << "Zadali jste:" << endl;
				cout << "ID: " << ucetniData[lengthData].ID << " |  " << ucetniData[lengthData].prijemVydaj << "\t" << ucetniData[lengthData].kategorie << "\t" << fixed << setprecision(0) << ucetniData[lengthData].castka << "\t" << date << endl;
				cout << endl;
			}
			else
			{
				ucetniData.pop_back();
			}

		}
		else
		{
			cout << "Nelze pridat dalsi polozku." << endl;
			ucetniData.pop_back();
			//break;
		}
}

/**
 * @brief Funkce prevede ruzne formaty datumu na jeden stejny
 * @param time - zadany cas
 * @param overallRows - pocet radku
 * @param errorText - struktura errorTextu
 * @param id - id pro errory
 * @param values - ucetnidata
 * @return true, pokud castka neprekrocila danou velikost, false pokud prekrocila 999999999.555
 */
void TimeFormat(string &time, unsigned int overallRows, ErrorText &errorText, int id, vector<UcetniData> &values)
{
	// funkce pro zavedeni jednotneho formatovani casu a kontrola, jestli vubec existuje
	Months m;
	string tmp;
	vector <string> newTime;
	unsigned int pos = 0;
	int numberOfDelimiters = 0;
	bool correctDate = true;
	bool wrongDay = false, wrongMonth = false;
	int day, month, year = 0;
	int completDate = 0;

	for (unsigned int i = 0; i < time.size() && correctDate; i++)
	{
		if (time[i] == ' ' || time[i] == '.' || time[i] == '-' || time[i] == ':')
		{
			tmp = time.substr(pos, i - pos);

			switch (numberOfDelimiters)
			{
			case 0:		// case 0 for days /DD/
				if (tmp.length() == 1 && tmp[0] != '0')  // 1 - 9 numbers only so alwazs correctDate
				{
					correctDate = true;
					day = stoi(tmp);
					completDate += 1;
				}
				else if (tmp.length() == 2 && tmp[0] == '0' && tmp[1] != '0')
				{
					if (stoi(tmp) >= 1) {
						correctDate = true;
						int i2 = tmp[1] - '0';
						day = i2;
						completDate += 1;
						//day = stoi(tmp);
					}
					else
						correctDate = false;
				}
				else if (tmp.length() == 2 && tmp[0] != '0')		// 00 - 30 +- 31
				{
					if (stoi(tmp) >= 10 && stoi(tmp) <= 31) {
						day = stoi(tmp);
						correctDate = true;
						completDate += 1;
					}
					else {
						correctDate = false;
						wrongDay = true;
					}

				}
				else if (tmp.length() > 2 || tmp.length() == 0)
				{
					correctDate = false;
					wrongDay = true;
				}
				else
					correctDate = false;
				break;

			case 1:		// case 1 for months /MM/  1-12 || 01 - 12
				if (tmp.length() == 1 && tmp[0] != '0')
				{
					correctDate = true;
					month = stoi(tmp);
					completDate += 1;
				}
				else if (tmp.length() == 2 && tmp[0] == '0' && tmp[1] != '0')
				{
					if (stoi(tmp) >= 1) {
						correctDate = true;
						int i2 = tmp[1] - '0';
						month = i2;
						completDate += 1;
						//month = stoi(tmp);
					}
				}
				else if (tmp.length() == 2 && tmp[0] != '0')
				{
					if (stoi(tmp) >= 10 && stoi(tmp) <= 12) {
						correctDate = true;
						month = stoi(tmp);
						completDate += 1;
					}
					else
					{
						correctDate = false;
						wrongMonth = true;
					}
				}
				else {
					correctDate = false;
				}
				break;

			default:
				break;
			}

			/*if (tmp.length() < 2) {
				tmp = '0' + tmp;
			}
			newTime.push_back(tmp);*/

			pos = i + 1;
			numberOfDelimiters++;
		}
		else if (numberOfDelimiters == 2)		// case for years /YYYY/ , only 1582 - 9999 years allowed
		{
			if (i < time.size() - 1) {
				year = stoi(time.substr(pos, time.size() - 1));

				if (year >= 1582 && year <= 9999) {
					correctDate = true;
					completDate += 1;
					break;
				}
				else
				{
					// spatny rok
					correctDate = false;
					break;
				}
		//newTime.push_back(time.substr(pos, time.size() - 1));
			}
		//break;
		}
		else {
			//correctDate = false; // nepouzivat! vsechny casy se daji na "spatny datum"
		}
	}

	if (correctDate && completDate == 3)
	{
		int correctDay;
		correctDay = m.NumOFDays(month, year);

		if (day <= correctDay)
		{
			// spravny datum
			string strDay, strMonth, strYear;

			if (to_string(day).length() < 2)
				strDay = "0" + to_string(day);
			else
				strDay = to_string(day);
			if (to_string(month).length() < 2)
				strMonth = "0" + to_string(month);
			else
				strMonth = to_string(month);

			strYear = to_string(year);

			values[overallRows].day = strDay;
			values[overallRows].month = strMonth;
			values[overallRows].year = strYear;
			//time = strDay + TIME_DELIMITER + strMonth + TIME_DELIMITER + strYear;
		}
		else {
			// Nespravny datum, kvuli prestupnemu roku
			time = "x";
			errorText.id.push_back(id);
			errorText.info.push_back("Neplatny rok --> prestupny rok");
			//to_string(overallRows) + " - Neplatny rok --> prestupny rok";
		}
	}
	else if (!correctDate && completDate == 3)
	{
		time = "x";
		errorText.id.push_back(id);
		errorText.info.push_back("Nespravne zadane datum.");
		//numOfErrorLines += 1;
		//ERROR_TEXT[numOfErrorLines] = "ID: " + to_string(overallRows) + " - Nespravne zadane datum";
	}
	else if (wrongDay) {
		time = "x";
		errorText.id.push_back(id);
		errorText.info.push_back("Neplatny rok --> Nespravne zadany den");
		//ERROR_TEXT[numOfErrorLines] = "ID: " + to_string(overallRows) + " - Neplatny rok --> Nespravne zadany den";
		//time = "Nespravne zadany den";
	}
	else if (wrongMonth)
	{
		time = "x";
		errorText.id.push_back(id);
		errorText.info.push_back("Neplatny rok --> Nespravne zadany mesic");
		//ERROR_TEXT[numOfErrorLines] = "ID: " + to_string(overallRows) + " - Neplatny rok --> Nespravne zadany mesic";
		//time = "Nespravne zadany mesic";
	}
	else if (year == NULL)
	{
		time = "x";
		errorText.id.push_back(id);
		errorText.info.push_back("Neplatny rok --> Chybi datum");
		//ERROR_TEXT[numOfErrorLines] = "ID: " + to_string(overallRows) + " - Neplatny rok --> Chybi datum";
		//time = "Chybi datum";
	}
	else
	{
		time = "x";
		errorText.id.push_back(id);
		errorText.info.push_back("Nespravne zadane datum.");
		//ERROR_TEXT[numOfErrorLines] = "ID: " + to_string(overallRows) + " - Nespravne zadane datum";
		//time = "Nespravne zadane datum";
	}
}

/**
 * @brief Funkce ktera zkousi prevest string na int, nebo double
 * @param num - zadany string pro prevod
 * @param c parametr pro c = 0 znaci prevod stringu na int, c = 3 prevod stringu na double
 * @return true, pokus lze string prevest na cislo, false pokud nelze
 */
bool TryConvertFromString(string num, int c)
{
	//int x;
	//double y;

	try {
		// do stuff that may throw or fail
		if(c == 0)
			stoi(num);
		else if (c == 3)
			stod(num);
	}
	catch (invalid_argument& e) {
		// if no conversion could be performed
		//cerr << "Invalid argument error: " << e.what() << endl;
		return false;
	}
	catch (out_of_range& ex) {
		// if the converted value would fall out of the range of the result type
		// or if the underlying function (std::strtol or std::strtoull) sets errno
		// to ERANGE.
		//cerr << "Error - out of range: " << ex.what() << endl;
		return false;
	}
	catch (...) {
		// catch any other errors (that we have no information about),everything else
		//cerr << "Unknown failure occurred." << endl;
		return false;
	}
	return true;
}

/**
 * @brief Funkce, ktera kontroluje spravnost ID
 * @param strId - strId je id k porovnani
 * @param ucetniData - data k porovnani s strId
 * @return true, pokud id neni obsazen v ucetnich datech, false ze uz tam je obsazen
 */
bool IsValidID(string strId, vector<UcetniData> ucetniData, ErrorText &errorText)
{
	bool isNumber;
	int id;
	isNumber = TryConvertFromString(strId);

	if (isNumber)
	{
		id = stoi(strId);
		if (IsIdDuplicated(id, ucetniData))
		{
			// error message --> duplicated IDs not allowed
			return false;
		}
		else if (id > 9999 || id < -1)
		{
			// error message --> ID is too low or too big
            errorText.id.push_back(id);
			errorText.info.push_back(id > 9999 ? "ID je vetsi jak 9999" : "ID je zaporne");
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		// error message --> ID is not a number
		return false;
	}
}

/**
 * @brief Funkce, ktera kontroluje duplicitni ID
 * @param id - id ke kontrole
 * @param ucetniData - data k porovnani s id
 * @return true, pokud id neni duplicitni s ostatnima id v ucetnichDatech, false pokud se jiz stejne id vyskytuje
 */
bool IsIdDuplicated(int id, vector<UcetniData> ucetniData)
{
	for (unsigned int i = 0; i < ucetniData.size(); i++)
	{
		if (id == ucetniData[i].ID)
			return true;
	}
	return false;
}

/**
 * @brief Funkce pro kontrolu, jestli vstup obsahuje "prijem", nebo "vydaj"
 * @param category string pro zkontrolovani
 * @return vraci true, pokud je category "prijem", nebo "vydaj", jinak vraci "x"
 */
string CheckIncomeExpenditure(string category)
{
	transform(category.begin(), category.end(), category.begin(), ::tolower);
	if (category == "prijem" || category == "vydaj")
	{
		return category;
	}
	else
	{
		return "x";
	}
}

/**
 * @brief Funkce pro vypis chyb
 * @param errorText je struktura s ulozenyma chybama
 */
void PrintErrors(ErrorText &errorText)
{
	cout << "Info:" << endl;
	cout << "Duplicitni ID jsou zaznamenana jako \"-1\". Prazdne, ci neplatne vyplnene pole jsou \"x\"." << endl;
	cout << "Radek, ktery obsahuje duplicitni ID, prazdne, ci nespravne vyplnene pole,\nnebude do html vypsan!" << endl << endl;
	cout << "Error list:" << endl;
	if (errorText.id.size() == 0)
		cout << "No Errors" << endl;
	else
	{
		for (unsigned int x = 0; x < errorText.id.size(); x++)
			cout << "Id: " << errorText.id[x] << "\t" << errorText.info[x] << endl;
	}
	cout << "---------------------------------------------------------------------------------------" << endl;
	cout << endl;

	//errorText.id.clear();
	//errorText.info.clear();
}

/**
 * @brief Funkce pro vytvoreni html souboru
 * @param data - vektor ucetnich dat
 */
void CreateHtml(vector<UcetniData> data)
{
	ofstream htmlfile;
	Months mnt;
	vector<UcetniData> sortedData;

	vector<double> inOut;
	vector<double> amount;
	vector<string> category;

	string htmlPath = (outputHtmlPath.length() != 0 ? outputHtmlPath : defaultOutputHtmlpath);
	htmlfile.open(htmlPath);
	htmlfile << "<!DOCTYPE html>\n<html>" << endl;
	htmlfile << "<head>\n<title>Ucetnictvi</title>\n</head>\n";
	htmlfile << "<body>\n";
	htmlfile << "<h1>Domaci ucetnictvi</h1>\n";

	while (data.size() != 0) // data sorting
	{
		unsigned int x = 0;
		unsigned int it = 0;
		string date1 = data[x].day + TIME_DELIMITER + data[x].month + TIME_DELIMITER + data[x].year;

		if (data[x].ID != -1 && data[x].prijemVydaj != "x" && data[x].kategorie.length() != 0 && SpacedMoneyValue(data[x].castka) != "x" && date1 != ".." && date1 != "::" && date1 != "--")
		{
			int m_year = stoi(data[it].year);
			int m_month = stoi(data[it].month);
			double m_money = data[it].castka;

			for (unsigned int j = 0; j < data.size(); j++)
			{
				string date2 = data[j].day + TIME_DELIMITER + data[j].month + TIME_DELIMITER + data[j].year;
				if (data[j].ID != -1 && data[j].prijemVydaj != "x" && data[x].kategorie.length() != 0 && SpacedMoneyValue(data[j].castka) != "x" && date2 != ".." && date2 != "::" && date2 != "--")
				{

					if (stoi(data[j].year) > m_year) //&& stoi(data[j].month) > m_month && data[it].castka > m_money
					{
						m_year = stoi(data[j].year);
						m_month = stoi(data[j].month);
						m_money = data[j].castka;
						it = j;
					}
					else if (stoi(data[j].year) == m_year)
					{
						if (stoi(data[j].month) > m_month)
						{
							m_year = stoi(data[j].year);
							m_month = stoi(data[j].month);
							m_money = data[j].castka;
							it = j;
						}
						else if (stoi(data[j].month) == m_month)
						{
							if (data[j].castka > m_money)
							{
								m_year = stoi(data[j].year);
								m_month = stoi(data[j].month);
								m_money = data[j].castka;
								it = j;
							}
							else if (data[j].castka == m_money)
							{
								m_year = stoi(data[j].year);
								m_month = stoi(data[j].month);
								m_money = data[j].castka;
								it = j;
							}
						}
					}
				}
				else
					data.erase(data.begin() + (int)j);
			}
			sortedData.push_back(data[it]);
			data.erase(data.begin() + (int)it);
		}
		else
			data.erase(data.begin() + (int)it);
	}

	int lastYear = 0;

	for (unsigned int g = 0; g < sortedData.size(); g++)
	{
		inOut.push_back(0);
		inOut.push_back(0);
		inOut.push_back(0);
		inOut.push_back(0);
		category.push_back("koupe");
		amount.push_back(0);

		int lastMonth = 0;
		int lstSaveMonth = 0;
		if (stoi(sortedData[g].year) == lastYear)
			continue;
		lastYear = stoi(sortedData[g].year);
		htmlfile << "<h2>" << sortedData[g].year << "</h2>\n";

		for (unsigned int x = 0; x < sortedData.size(); x++)
		{
			bool isHereMonth = false;
			if (stoi(sortedData[x].month) == lstSaveMonth || stoi(sortedData[x].year) != lastYear)
				continue;

			lastMonth = stoi(sortedData[x].month);

			for (unsigned int k = 0; k < sortedData.size(); k++)
			{
				if (stoi(sortedData[k].month) == lastMonth && stoi(sortedData[k].year) == lastYear)
				{
					isHereMonth = true;
					break;
				}
			}

			if(isHereMonth)
			{
				//auto it = max_element(begin(data), end(data));
				int srtMonth = stoi(sortedData[x].month);
				if (sortedData[x].month.length() == 2 && sortedData[x].month[0] == 0)
					srtMonth = int(sortedData[x].month[1]);

				htmlfile << "<h3><i><b>" << string(mnt.nazvyMesicu[srtMonth - 1]) << "</b></i></h3>" << endl;
				htmlfile << "<p>Serazeno dle nejvyssi castky</p>";
				htmlfile << "<table border = \"1\">\n";
				htmlfile << "	<tr>\n";
				htmlfile << "		<th>ID</th>\n";
				htmlfile << "		<th>Typ</th>\n";
				htmlfile << "		<th>Kategorie</th>\n";
				htmlfile << "		<th>Castka [Kc]</th>\n";
				htmlfile << "		<th>Datum</th>\n";
				htmlfile << "	</tr>" << endl;

				for (unsigned int i = 0; i < sortedData.size(); i++)
				{
					if (stoi(sortedData[i].month) == lastMonth && stoi(sortedData[i].year) == lastYear)
					{
						string date = sortedData[i].day + TIME_DELIMITER + sortedData[i].month + TIME_DELIMITER + sortedData[i].year;
						htmlfile << "	<tr>\n";
						htmlfile << "		<td>" << sortedData[i].ID << "</td>\n";
						htmlfile << "		<td>" << sortedData[i].prijemVydaj << "</td>\n";
						htmlfile << "		<td>" << sortedData[i].kategorie << "</td>\n";
						htmlfile << "		<td>" << SpacedMoneyValue(sortedData[i].castka) << "</td>\n";
						htmlfile << "		<td>" << date << "</td>\n";
						htmlfile << "	</tr>\n";

						if (sortedData[i].prijemVydaj == "prijem")
						{
							inOut[0] += sortedData[i].castka;
							inOut[2] += sortedData[i].castka;
						}
						else
						{
							inOut[1] += sortedData[i].castka;
							inOut[3] += sortedData[i].castka;
						}

						unsigned int l;
						for (l = 0; l < category.size(); l++)
						{
							if (category[l] == sortedData[i].kategorie)
							{
								amount[l] += sortedData[i].castka;
								break;
							}
						}
						if (l >= category.size())
						{
							category.push_back(sortedData[i].kategorie);
							amount.push_back(sortedData[i].castka);
						}
					}
				}
				htmlfile << "</table>\n" << endl;
				htmlfile << "<p><b>Celkem za mesic</b></p>";
				htmlfile << "<table border = \"1\">\n";
				htmlfile << "	<tr>\n";
				for (unsigned int l = 0; l < category.size(); l++)
				{
					if(amount[l] != 0)
						htmlfile << "		<th>" << category[l] << "</th>\n";
				}
				htmlfile << "	</tr>" << endl;
				htmlfile << "	<tr>\n";
				for (unsigned int l = 0; l < category.size(); l++)
				{
					if (amount[l] != 0)
						htmlfile << "		<td>" << SpacedMoneyValue(amount[l]) << "</td>\n";
				}
				htmlfile << "	</tr>\n";
				htmlfile << "</table>" << endl;
				htmlfile << "<br>\n";
				htmlfile << "<table border = \"1\">\n";
				htmlfile << "	<tr>\n";
				htmlfile << "		<th>Prijem</th>\n";
				htmlfile << "		<th>Vydaj</th>\n";
				htmlfile << "		<th>Celkem</th>\n";
				htmlfile << "	</tr>" << endl;
				htmlfile << "	<tr>\n";
				htmlfile << "		<td>" << SpacedMoneyValue(inOut[0]) << "</td>\n";
				htmlfile << "		<td>" << SpacedMoneyValue(inOut[1]) << "</td>\n";
				htmlfile << "		<td>" << SpacedMoneyValue(inOut[0] - inOut[1]) << "</td>\n";
				htmlfile << "	</tr>\n";
				htmlfile << "</table>" << endl;

				lstSaveMonth = lastMonth;
			}
			lastMonth = lstSaveMonth;
			inOut[0] = 0;
			inOut[1] = 0;
		}

		htmlfile << "<p><b>Celkem za rok</b></p>";
		//htmlfile << "<br>\n";
		htmlfile << "<table border = \"1\">\n";
		htmlfile << "	<tr>\n";
		htmlfile << "		<th>Prijem</th>\n";
		htmlfile << "		<th>Vydaj</th>\n";
		htmlfile << "		<th>Celkem</th>\n";
		htmlfile << "	</tr>" << endl;
		htmlfile << "	<tr>\n";
		htmlfile << "		<td>" << SpacedMoneyValue(inOut[2]) << "</td>\n";
		htmlfile << "		<td>" << SpacedMoneyValue(inOut[3]) << "</td>\n";
		htmlfile << "		<td>" << SpacedMoneyValue(inOut[2] - inOut[3]) << "</td>\n";
		htmlfile << "	</tr>\n";
		htmlfile << "</table>" << endl;

		inOut.clear();
		amount.clear();
		category.clear();
	}

	//ending html
	htmlfile << "</tbody>\n";
	htmlfile << "</body>\n</html>";
	htmlfile.close();
}
