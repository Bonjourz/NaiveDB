#include "Client.h"
#include <ctime>
using namespace std;


Client::Client()
{
	// Get the option from the users
	while (true) {
		cout << "1. Test Correctness."
			<< endl << "2. Test Performance."
			<< endl << "3. Use Database. "
			<< endl << "Your choice(-1 to quit):";
		int choice;
		cin >> choice;
		if (choice == 1)
			testForCorrectness(1000000);

		else if (choice == 2)
			testPerformance();

		else if (choice == 3)
			welcome();

		else if (choice == -1)
			exit(0);
	}
}


Client::~Client()
{
	delete db;
}

void Client::testPerformance() {
	// Get the option from users
	while (true) {
		cout << "1. Test cache"
			<< endl << "2. Test num of data"
			<< endl << "3. Test size of value"
			<< endl << "4. Test operation"
			<< endl << "Your choice(-1 to quit): ";

		int choice;
		cin >> choice;
		if (choice == 1)
			for (int i = 0; i < 10; i++)
				testCache(10000000, i);

		else if (choice == 2) {
			testPerformance(1000000, 5, 10);
			testPerformance(5000000, 5, 10);
			testPerformance(10000000, 5, 10);
			testPerformance(20000000, 5, 10);
			testPerformance(30000000, 5, 10);
			testPerformance(4000000, 5, 10);
			testPerformance(5000000, 5, 10);
		}

		else if (choice == 3)
			for (int i = 2; i < 12; i++)
				testPerformance(4000000, 2, i * 10);

		else if (choice == 4)
			for (int i = 0; i < 4; i++) {
				testOperating(100000, i);
				testOperating(500000, i);
				testOperating(1000000, i);
				testOperating(2000000, i);
				testOperating(3000000, i);
				testOperating(4000000, i);
				testOperating(5000000, i);
				testOperating(6000000, i);
				testOperating(7000000, i);
				testOperating(8000000, i);
				testOperating(9000000, i);

			}

		else if (choice == -1)
			return;
	}
}

void Client::welcome() {
	// Get the option from users
	cout << "Please input the name of database:";
	cin >> dbName;
	db = new DB(dbName);
	int choice;
	while (true) {
		cout << "1. Load the data and configuration." << endl
			<< "2. New a database." << endl
			<< "Your choice(-1 to quit): ";
		cin >> choice;
		if (choice == -1)
			return;

		// Load the configuration from the disk
		else if (choice == 1) {
			db->loadFromFile();
			recordSize = db->getRecordSize();
			cacheSize = db->getCacheSize();
			break;
		}

		// New a database
		else if (choice == 2) {
			getConfigure();
			db->newDB(cacheSize, recordSize);
			break;
		}
	}
	run();
}

void Client::run() {
	// Get the option from users
	while (true) {
		cout << endl << "1. Fecth Record"
			<< endl << "2. Modify Record"
			<< endl << "3. Insert Record"
			<< endl << "4. Delete Record"
			<< endl << "5. Clear data file"
			<< endl << "6. Configuration of database"
			<< endl << "Your choice(-1 to quit): ";

		int choice;
		cin >> choice;
		if (choice == 1)
			fetchRecord();

		else if (choice == 2)
			modifyRecord();

		else if (choice == 3)
			insertRecord();

		else if (choice == 4)
			deleteRecord();

		else if (choice == 5)
			db->clearDat();

		else if (choice == 6) {
			cout << "CacheSize: " << cacheSize
				<< endl << "RecordSize: " << recordSize
				<< endl << "Size: " << db->getDBSize() << endl;
		}

		// Ask the user whether store the information to the disk
		else if (choice == -1) {
			while (true) {
				cout << "Do you want to store the data(y/n)";
				char opt;
				cin >> opt;
				if (opt == 'y' || opt == 'Y') {
					db->close();
					delete db;
					return;
				}
				else if (opt == 'n' || opt == 'N') {
					delete db;
					return;
				}
			}
		}
	}
}

void Client::fetchRecord() {
	KEYTYPE key;
	cout << "Please input key: ";
	cin >> key;
	VALUETYPE value;
	// If cannot find the record
	if (!db->fetchRecord(key, value))
		cout << "Cannnot find record" << endl;

	else
		cout << "The record is: " << value << endl;
}
void Client::insertRecord() {
	KEYTYPE key;
	cout << "Please input key: ";
	cin >> key;
	VALUETYPE value;
	cout << "Please input value: ";
	cin >> value;
	// If the size of record is larger than the maximum
	if (value.size() > recordSize) {
		cout << "The length of record is to long!" << endl;
		return;
	}

	if (!db->insertRecord(key, value))
		cout << "Duplicate key!" << endl;

	else
		cout << "Insert Success!" << endl;
}
void Client::modifyRecord() {
	KEYTYPE key;
	cout << "Please intput key: ";
	cin >> key;
	VALUETYPE value;
	cout << "Please input value: ";
	cin >> value;
	// If the size of record is larger than the maximum
	if (value.size() > recordSize) {
		cout << "The length of record is to long!" << endl;
		return;
	}

	if (!db->modify(key, value))
		cout << "The key doesn't exist" << endl;

	else
		cout << "Success!" << endl;

}
void Client::deleteRecord() {
	KEYTYPE key;
	cout << "Please input key: ";
	cin >> key;
	if (!db->deleteRecord(key))
		cout << "The key doesn't exist!" << endl;

	else
		cout << "Success!" << endl;
}

void Client::test() {
	/* Empty */
}

void Client::getConfigure() {
	bool flag1, flag2;
	do {
		flag1 = true;
		flag2 = true;
		int sizeOfCache;
		cout << "Please input the size of cache: ";
		// Get the size of cache from users
		cin >> cacheSize;
		if (cacheSize > 0)
			flag1 = false;

		cout << "Please input the size of record: ";
		cin >> recordSize;
		if (recordSize > 0)
			flag2 = false;

	} while (flag1 || flag2);
}

void Client::testForCorrectness(const int& nrec) {
	srand(time(NULL));
	DB *db = new DB("database");
	db->newDB(0, 15);
	VALUETYPE value = "testRecord";
	
	for (int i = 0; i < nrec; i++) {
		if (i % (nrec / 50) == 0)
			value[value.size() - 1] = (char)(rand() % 128);

		// Generalize the key randomly
		KEYTYPE key = rand() * rand();
		db->insertRecord(key, value);
		VALUETYPE result = "";
		if (!db->fetchRecord(key, result))
			cout << "Fetch error1!" << endl;

		if (result != value) {
			cout << "Fetch error2!" << value << result << "a";
		}

		if (!db->deleteRecord(key))
			cout << "Delete error3!" << endl;

		if (db->fetchRecord(key, result))
			cout << "Fetch error4!" << endl;
	}
	delete db;
}

void Client::testPerformance(const int& nrec, const int& cacheSize,
	const int& recordSize) {
	db = new DB("database");
	db->newDB(cacheSize, recordSize);
	int ins = 0, del = 0, fet = 0;
	srand(time(NULL));
	int time1 = clock();
	string testRecord = "test";
	string replace = "replace";

	// Insert record
	for (int i = 0; i < nrec; i++) {
		db->insertRecord((KEYTYPE)i, testRecord);
		ins++;
	}

	srand(time(NULL));

	VALUETYPE value;
	for (int i = 0; i < nrec; i++) {
		value = "";
		if (!db->fetchRecord(i, value))
			cout << "Fetch error!" << endl;

		if (value != testRecord)
			cout <<  "Fetch error!" << endl;

		fet++;
	}

	for (int i = 0; i < nrec * 5; i++) {
		db->fetchRecord(i, value);
		fet++;
		
		if (i % 37 == 0) {
			db->deleteRecord((INT_MAX * rand()) % nrec);
			del++;
		}

		if (i % 11 == 0) {
			KEYTYPE key = (INT_MAX * rand()) % nrec;
			if (!db->insertRecord(key, testRecord))
				db->modify(key, testRecord);
			ins++;

			value = "";
			if (!db->fetchRecord(key, value))
				cout << value << endl;

			if (value != testRecord) {
				cout << "Fetch error4 " << endl;
				exit(0);
			}

			fet++;
		}

		if (i % 17 == 0) {
			if (i % 34 == 0)
				db->modify(i, replace + replace);

			else
				db->modify(i, replace + replace);

			ins++;
		}
	}

	for (int i = 0; i < nrec; i++) {
		db->deleteRecord(i);
		del++;
		for (int j = 0; j < 10; j++) {
			db->fetchRecord(rand() % nrec, value);
			fet++;
		}
	}

	int time2 = (clock() - time1) / CLOCKS_PER_SEC;
	cout << "insert:" << ins << endl;
	cout << "delete:" << del << endl;
	cout << "search:" << fet << endl;
	cout << "cache size:" << cacheSize << endl;
	cout << "time:" << time2 << " seconds\n\n";
	delete db;
}

void Client::testCache(const int& nrec, const int& cacheSize) {
	DB* db = new DB("database");
	db->newDB(cacheSize, 20);
	VALUETYPE value = "test";
	VALUETYPE result;
	clock_t time1 = clock();
	for (int i = 0; i < nrec / 10; i++) {
		for (int j = 0; j < 10; j++) {
			db->insertRecord((KEYTYPE)(i + j), value);
		}

		result = "";
		// Fetch record
		for (int k = 0; k < 1000; k++) {
			db->fetchRecord(i + rand() % 10, result);
		}
	}
	clock_t time2 = clock() - time1;
	cout << "time: " << time2 << " ms\n\n";
	delete db;
}

void Client::testOperating(const int& nrec, const int& flag) {
	DB *db = new DB("database");
	db->newDB(2, 8);
	VALUETYPE value = "test";
	for (int i = 0; i < nrec; i++)
		db->insertRecord(i, value);

	clock_t time1 = clock();
	string info = "";
	// Get the users option
	if (flag == 0) {
		for (int i = 0; i < nrec; i++) 
			db->fetchRecord(i, value);
		info = "Fetch";
	}

	else if (flag == 1) {
		for (int i = 0; i < nrec; i++)
			db->modify(i, value);
		info = "Modify";
	}

	else if (flag == 2) {
		for (int i = 0; i < nrec; i++)
			db->insertRecord(i, value);
		info = "Insert";
	}

	else if (flag == 3) {
		for (int i = 0; i < nrec; i++)
			db->deleteRecord(i);
		info = "Delete";
	}

	clock_t time2 = clock() - time1;
	cout << "Operation for " << info
		<< " time: " << time2 << " ms\n\n";
	delete db;
}