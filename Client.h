#pragma once
#include "DB.h"
class Client
{
public:
	Client();
	~Client();
	string dbName;

private:
	DB *db;

	/* The size of cache of the database */
	int cacheSize;

	/* The max bytes of record size */
	int recordSize;

	/* Get the configuration of the database from user */
	void getConfigure();

	void test();

	/* Test the correctness of the database */
	void testForCorrectness(const int& nrec);

	/* Test the performance of the database */
	void testPerformance(const int& nrec, const int& cacheSize,
		const int& recordSize);

	/* Test the performance about cache of teh databse */
	void testCache(const int& nrec, const int& cacheSize);

	/* Test the performance of the operating of insert, 
	** delete, modify and fetch
	** flag = 0 : fetch
	** flag = 1 : modify
	** flag = 2 : insert
	** flag = 3 : delete
	*/
	void testOperating(const int& nrec, const int& flag);

	/* Get the input of the users option */
	void testPerformance();

	/* Welcome for users */
	void welcome();

	/* Fetch record from the database */
	void fetchRecord();

	/* Insert record to the database */
	void insertRecord();

	/* Modify record of the database */
	void modifyRecord();

	/* Delete the record from the database */
	void deleteRecord();

	/* Run the database for the user */
	void run();
};

