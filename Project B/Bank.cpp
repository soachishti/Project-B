#ifndef Bank_CPP
#define Bank_CPP

#include <iostream>
#include <string>     // to_string()
#include <sstream>
#include <iomanip>
#include <ctime>      // time()
#include <fstream>    // ofstream

#include "Graphics.cpp"

#pragma warning(disable : 4996)

using namespace std;

struct userDetail {
	string name;
	string username;
	string password;
	int pin;
	short int role;
	bool atmBlocked;
};

/*
Use camelCase in naming functions
*/
class Bank : public Graphics
{
public:
	userDetail user;
	char infoFile[20];
	char dataDir[20];
	bool loggedIn = false;
	ofstream infoFileWriteStream;

	userDetail currentUser;

	Bank()
	{
		srand((unsigned int)time(NULL));
		strcpy_s(infoFile, "infoFile.txt");
		strcpy_s(dataDir, "data/");
		bool loggedIn = false;
		infoFileWriteStream.open(infoFile, ios::app);
	}

	userDetail getRoleValue(string username) {
		fstream infoFileReadStream(infoFile);
		userDetail user;
		//int position = 0;
		while (infoFileReadStream
			>> user.username
			>> user.password
			>> user.pin
			>> user.role
			>> user.atmBlocked
			)
		{
			if (user.username == username) {
				return user;
			}
			//position = infoFileReadStream.tellg();
		}
		return user;
	}

	bool putCurrentUserValue(string username, userDetail data) {

		fstream infoFileReadStream(infoFile);
		userDetail user;
		streamoff position = 0;
		while (infoFileReadStream
			>> user.username
			>> user.password
			>> user.pin
			>> user.role
			>> user.atmBlocked
			)
		{
			if (user.username == username) {
				infoFileReadStream.seekp(static_cast<int>(position));
				infoFileReadStream << data.username << " "
					<< data.password << " "
					<< data.pin << " "
					<< data.role << " "
					<< data.atmBlocked
					<< endl;
				break;
			}
			position = infoFileReadStream.tellg();
		}
		return 0;
	}

	bool changePIN(string username, int pin){
		userDetail data = getRoleValue(username);
		data.pin = pin;
		putCurrentUserValue(username, data);
		return true;
	}

	bool changePassword(string username, string password){
		userDetail data = getRoleValue(username);
		data.password = password;
		putCurrentUserValue(username, data);
		return true;
	}

	bool unlockATM(string username, bool unlock){
		userDetail data = getRoleValue(username);
		data.atmBlocked = unlock;
		putCurrentUserValue(username, data);
		return true;
	}

	void deleteUser(string username){
		userDetail data = getRoleValue(username);
		data.name = "DEL_N";
		data.username = "DEL_UN";
		data.password = "DEL_P";
		data.pin = -1;
		data.role = -1;
		data.atmBlocked = 0;
		putCurrentUserValue(username, data);

		string fileLoc = dataDir + username;
		remove(fileLoc.c_str());
	}

	static int randomPIN()
	{
		int min = 1111;
		int max = 9999;
		int randNo = ((rand() % (max - min)) + min);
		srand(randNo + (unsigned int)time(NULL));
		return randNo;
	}

	string repeatCh(char repeatedCh, int limit) {
		string data;
		for (int i = 0; i < limit; i++) {
			data += repeatedCh;
		}
		return data;
	}

	void modifyKeyValue(string username, string key, string value) {
		string userFileLoc = string(dataDir) + username + ".txt";
		fstream userFile(userFileLoc.c_str());
		string tag, info;
		streamoff position = 0;
		while (userFile >> tag >> info) {
			if (tag == key) {
				userFile.seekp(static_cast<int>(position));
				userFile << repeatCh(' ', info.size()+2); // Delete Previous Data
				userFile.seekp(static_cast<int>(position));

				userFile << endl << key << " " << value << endl;
				break;
			}
			position = userFile.tellg();

		}
		userFile.close();
	}

	string getValueByKey(string username, string key){
		string userFileLoc = string(dataDir) + username + ".txt";
		fstream userFile(userFileLoc.c_str());
		string tag, info;
		while (userFile >> tag >> info) {
			if (tag == key) {
				return info.c_str();
			}
		}
		userFile.close();
		return "NULL";
	}

	double getBalance(string username) {
		string key = "BAL";
		string value = getValueByKey(username, key);
		return atof(value.c_str());
	}

	bool addBalance(string username, double amount) {
		double finalAmount = getBalance(username) + amount;
		string balance = to_string_precision(finalAmount);
		modifyKeyValue(username, "BAL", balance);
		return true;
	}

	bool usernameAlreadyExist(string username) {
		string line;
		userDetail user;
		ifstream infoFileReadStream(infoFile);
		while (infoFileReadStream
			>> user.username
			>> user.password
			>> user.pin
			>> user.role
			>> user.atmBlocked
			)
		{
			if (user.username == username) {
				return true;
			}
		}
		infoFileReadStream.close();
		return false;
	}

	string replaceWith(string str, char pin, char replace) {
		string newStr;
		int size = str.size();
		for (int i = 0; i < size; i++){
			if (str.c_str()[i] == pin) {
				newStr += replace;
			}
			else {
				newStr += str.c_str()[i];
			}
		}
		return newStr;
	}

	bool isBlocked(string username) {
		string line;
		bool flag = false;
		userDetail userData;
		ifstream infoFileReadStream(infoFile);
		while (infoFileReadStream
			>> userData.username
			>> userData.password
			>> userData.pin
			>> userData.role
			>> userData.atmBlocked
			)
		{
			if (userData.username == username) {
				flag = userData.atmBlocked;
				break;
			}
		}
		infoFileReadStream.close();
		return flag;
	}

	bool login(string username, string password, int role) {
		string line;
		bool flag = false;
		userDetail userData;
		ifstream infoFileReadStream(infoFile);
		while (infoFileReadStream
			>> userData.username
			>> userData.password
			>> userData.pin
			>> userData.role
			>> userData.atmBlocked
			)
		{
			if (userData.username == username &&
				userData.password == password &&
				userData.role == role
				) {
				flag = true;
				currentUser = userData; // Login user
				currentUser.name = replaceWith(getValueByKey(username, "NAME"), '_', ' ');
				break;
			}
		}
		infoFileReadStream.close();
		return flag;
	}

	bool loginWithPIN(string username, string pin, short int role) {
		string line;
		bool flag = false;
		userDetail userData;
		ifstream infoFileReadStream(infoFile);
		while (infoFileReadStream
			>> userData.username
			>> userData.password
			>> userData.pin
			>> userData.role
			>> userData.atmBlocked
			)
		{
			if (userData.username == username && userData.pin == atoi(pin.c_str()) && userData.role == role) {
				flag = true;
				currentUser = userData; // Login user
				currentUser.name = replaceWith(getValueByKey(username, "NAME"), '_', ' ');
				break;
			}
		}
		infoFileReadStream.close();
		return flag;
	}

	bool addUser(userDetail user) {
		// Checking whether person with same username exist or not
		// return false and cancel all operation
		if (usernameAlreadyExist(user.username))
		{
			return false;
		}

		// Adding data to file
		infoFileWriteStream << "\n" << user.username << " "
			<< user.password << " "
			<< user.pin << " "
			<< user.role << " "
			<< user.atmBlocked
			<< endl;

		// Create file for user
		string userFileLoc = string(dataDir) + user.username + ".txt";
		ofstream userFile(userFileLoc.c_str());

		user.name = replaceWith(user.name, ' ', '_');

		time_t rawtime;
		time(&rawtime);
		string CDtime = replaceWith(ctime(&rawtime), ' ', '_');

		userFile << "NAME " << user.name << endl
			<< "CD " << CDtime << endl
			<< "BAL " << 0 << endl;
		userFile.close();

		return true;
	}

	bool isLogged() {
		if (currentUser.username.size() > 0)
			return true;
		else
			return false;

	}

	bool logout() {
		currentUser.name = "";
		currentUser.username = "";
		currentUser.password = "";
		currentUser.pin = -1;
		currentUser.role = -1;
		currentUser.atmBlocked = 0;
		return true;
	}

	string creationData() {
		string key = "CD";
		string value = getValueByKey(currentUser.username, key);
		return replaceWith(value, '_', ' ');
	}

	~Bank()
	{
		infoFileWriteStream.close();
	}
};

#endif