
#ifndef DBMANAGER
#define DBMANAGER

#include <cstdlib>
#include <string>
#include <sqlite3.h>
#include "Types.h"

class DBManager{
private:
    sqlite3 *db;
    int rc;
public:
    DBManager(std::string filename);
    ~DBManager();
    void insert(FlowDBRecord& fr);
};
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
DBManager::DBManager(std::string filename){
    char *zErrMsg = 0;

    rc = sqlite3_open(filename.c_str(), &db);

    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    /* Create SQL statement */
    char *sql = "CREATE TABLE IF NOT EXISTS `NETFLOW` ("\
                "`srcIP` VARCHAR(45) NOT NULL,"\
                "`desIP` VARCHAR(45) NOT NULL,"\
                "`protocol` INT NOT NULL,"\
                "`srcPort` INT NOT NULL,"\
                "`desPort` INT NOT NULL,"\
                "`typeOfService` INT NOT NULL,"\
                "`size` INT NULL,"\
                "`startTime` DATETIME NOT NULL,"\
                "`duration` REAL NULL,"\
                "`packets` INT NULL,"\
                "PRIMARY KEY (`srcIP`, `desIP`, `protocol`, `srcPort`, `desPort`, `typeOfService`, `startTime`));";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Table created successfully\n");
    }
}
DBManager::~DBManager(){
    sqlite3_close(db);
}
void DBManager::insert(FlowDBRecord& fdbr){
    char *zErrMsg = 0;

    std::string values = "INSERT INTO NETFLOW (srcIP,desIP,protocol,srcPort,desPort,typeOfService,size,startTime,duration,packets) ";
    values += "VALUES ('" + fdbr.srcIP + "', '" + fdbr.desIP + "', " + std::to_string(fdbr.protocol) + ", " + std::to_string(fdbr.srcPort)
    + ", " + std::to_string(fdbr.desPort) + ", " + std::to_string(fdbr.typeOfService) + ", " + std::to_string(fdbr.size) + ", datetime('" + fdbr.startTime + "'), " + std::to_string(fdbr.duration) + ", " + std::to_string(fdbr.packets) + ");";
    /* Create SQL statement */
    const char *sql = values.c_str();

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Records created successfully\n");
    }
}

#endif