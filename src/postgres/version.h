/***********************************************************************************************************************************
PostreSQL Version Constants
***********************************************************************************************************************************/
#ifndef POSTGRES_VERSION_H
#define POSTGRES_VERSION_H

/***********************************************************************************************************************************
PostgreSQL name
***********************************************************************************************************************************/
#define PG_NAME                                                     "PostgreSQL"

/***********************************************************************************************************************************
PostgreSQL version constants
***********************************************************************************************************************************/
#define PG_VERSION_83                                                80300
#define PG_VERSION_84                                                80400
#define PG_VERSION_90                                                90000
#define PG_VERSION_91                                                90100
#define PG_VERSION_92                                                90200
#define PG_VERSION_93                                                90300
#define PG_VERSION_94                                                90400
#define PG_VERSION_95                                                90500
#define PG_VERSION_96                                                90600
#define PG_VERSION_10                                               100000
#define PG_VERSION_11                                               110000

#define PG_VERSION_MAX                                              PG_VERSION_11

/***********************************************************************************************************************************
PostgreSQL version string constants for use in error messages
***********************************************************************************************************************************/
#define PG_VERSION_83_STR                                            "8.3"
#define PG_VERSION_84_STR                                            "8.4"
#define PG_VERSION_90_STR                                            "9.0"
#define PG_VERSION_91_STR                                            "9.1"
#define PG_VERSION_92_STR                                            "9.2"
#define PG_VERSION_93_STR                                            "9.3"
#define PG_VERSION_94_STR                                            "9.4"
#define PG_VERSION_95_STR                                            "9.5"
#define PG_VERSION_96_STR                                            "9.6"
#define PG_VERSION_10_STR                                            "10"
#define PG_VERSION_11_STR                                            "11"

#endif
