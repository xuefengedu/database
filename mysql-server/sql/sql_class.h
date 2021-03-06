/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


/* Classes in mysql */

#ifdef __GNUC__
#pragma interface			/* gcc class implementation */
#endif


class Query_log_event;
class Load_log_event;


enum enum_duplicates { DUP_ERROR, DUP_REPLACE, DUP_IGNORE };
enum enum_log_type { LOG_CLOSED, LOG_NORMAL, LOG_NEW, LOG_BIN };

// log info errors 
#define LOG_INFO_EOF -1
#define LOG_INFO_IO  -2
#define LOG_INFO_INVALID -3
#define LOG_INFO_SEEK -4

typedef struct st_log_info
{
  char log_file_name[FN_REFLEN];
  my_off_t index_file_offset;
  my_off_t pos; 
} LOG_INFO;

typedef struct st_master_info
{
  char log_file_name[FN_REFLEN];
  ulong pos;
  FILE* file; // we keep the file open, so we need to remember the file pointer

  // the variables below are needed because we can change masters on the fly
  char host[HOSTNAME_LENGTH+1];
  char user[USERNAME_LENGTH+1];
  char password[HASH_PASSWORD_LENGTH+1];
  uint port;
  uint connect_retry;
  pthread_mutex_t lock;
  bool inited;

  st_master_info():inited(0) { host[0] = 0; user[0] = 0; password[0] = 0;}
  inline void inc_pos(ulong val)
  {
    pthread_mutex_lock(&lock);
    pos += val;
    pthread_mutex_unlock(&lock);
  }
  // thread safe read of position - not needed if we are in the slave thread,
  // but required otherwise
  inline void read_pos(ulong& var)
  {
    pthread_mutex_lock(&lock);
    var = pos;
    pthread_mutex_unlock(&lock);
  }
} MASTER_INFO;

class MYSQL_LOG {
 public:
 private:
  pthread_mutex_t LOCK_log, LOCK_index;
  FILE *file, *index_file;
  time_t last_time,query_start;
  char *name;
  enum_log_type log_type;
  char time_buff[20],db[NAME_LEN+1];
  char log_file_name[FN_REFLEN],index_file_name[FN_REFLEN];
  bool write_error,inited;

public:
  MYSQL_LOG();
  ~MYSQL_LOG();
  pthread_mutex_t* get_log_lock() { return &LOCK_log; }
  void set_index_file_name(const char* index_file_name = 0);
  void open(const char *log_name,enum_log_type log_type,
	    const char *new_name=0);
  void new_file(void);
  void write(enum enum_server_command command,const char *format,...);
  void write(const char *query, uint query_length, ulong time_to_do_query=0);
  void write(Query_log_event* event_info); // binary log write
  void write(Load_log_event* event_info);

  int generate_new_name(char *new_name,const char *old_name);
  void make_log_name(char* buf, const char* log_ident);
  bool is_active(const char* log_file_name);
  void flush(void);
  void close(bool exiting = 0); // if we are exiting, we also want to close the
  // index file

  // iterating through the log index file
  int find_first_log(LOG_INFO* linfo, const char* log_name);
  int find_next_log(LOG_INFO* linfo);
  int get_current_log(LOG_INFO* linfo);

  bool is_open() { return log_type != LOG_CLOSED; }
  char* get_index_fname() { return index_file_name;}
  char* get_log_fname() { return log_file_name; }
};

/* character conversion tables */

class CONVERT
{
  const uchar *from_map,*to_map;
  void convert_array(const uchar *mapping,uchar *buff,uint length);
public:
  const char *name;
  CONVERT(const char *name_par,uchar *from_par,uchar *to_par)
    :from_map(from_par),to_map(to_par),name(name_par) {}
  friend CONVERT *get_convert_set(const char *name_ptr);
  inline void convert(char *a,uint length)
  {
    convert_array(from_map, (uchar*) a,length);
  }
  bool store(String *, const char *,uint);
};

typedef struct st_copy_info {
  ha_rows records;
  ha_rows deleted;
  ha_rows copied;
  ha_rows error;
  enum enum_duplicates handle_duplicates;
  int escape_char;
} COPY_INFO;


class key_part_spec :public Sql_alloc {
public:
  const char *field_name;
  uint length;
  key_part_spec(const char *name,uint len=0) :field_name(name), length(len) {}
};


class Alter_drop :public Sql_alloc {
public:
  enum drop_type {KEY, COLUMN };
  const char *name;
  enum drop_type type;
  Alter_drop(enum drop_type par_type,const char *par_name)
    :name(par_name), type(par_type) {}
};


class Alter_column :public Sql_alloc {
public:
  const char *name;
  Item *def;
  Alter_column(const char *par_name,Item *literal)
    :name(par_name), def(literal) {}
};


class Key :public Sql_alloc {
public:
  enum Keytype { PRIMARY, UNIQUE, MULTIPLE, FULLTEXT };
  enum Keytype type;
  List<key_part_spec> columns;
  const char *Name;

  Key(enum Keytype type_par,const char *name_arg,List<key_part_spec> &cols)
    :type(type_par), columns(cols),Name(name_arg) {}
  ~Key() {}
  const char *name() { return Name; }
};


typedef struct st_mysql_lock
{
  TABLE **table;
  uint table_count,lock_count;
  THR_LOCK_DATA **locks;
} MYSQL_LOCK;


class LEX_COLUMN : public Sql_alloc
{
public:
  String column;
  uint rights;
  LEX_COLUMN (const String& x,const  uint& y ): column (x),rights (y) {}
};

#include "sql_lex.h"				/* Must be here */

// needed to be able to have an I_List of char* strings.in mysqld.cc where we cannot use String
// because it is Sql_alloc'ed
class i_string: public ilink
{
public:
  char* ptr;
  i_string():ptr(0) { }
  i_string(char* s) : ptr(s) {}
};

/****************************************************************************
** every connection is handle by a thread with a THD
****************************************************************************/

class delayed_insert;

class THD :public ilink {
public:
  NET	  net;
  LEX	  lex;
  MEM_ROOT alloc;
  HASH     user_vars;
  String  packet;				/* Room for 1 row */
  struct  sockaddr_in remote;
  struct  rand_struct rand;
  char	  *query,*thread_stack;
  char	  *host,*user,*priv_user,*db,*ip;
  const   char *proc_info;
  uint	  client_capabilities,max_packet_length;
  uint	  master_access,db_access;
  TABLE   *open_tables,*temporary_tables;
  MYSQL_LOCK *lock,*locked_tables;
  ULL	  *ull;
  struct st_my_thread_var *mysys_var;
  enum enum_server_command command;
  const char *where;
  char* last_nx_table; // last non-existent table, we need this for replication
  char* last_nx_db; // database of the last nx table
  time_t  start_time;
  time_t  connect_time,thr_create_time; // track down slow pthread_create
  thr_lock_type update_lock_default;
  delayed_insert *di;
  struct st_transactions {
    void *bdb_tid;
    uint bdb_lock_count;
  } transaction;
  Item	     *free_list;
  CONVERT    *convert_set;
  Field      *dupp_field;
#ifndef __WIN__
  sigset_t signals,block_signals;
#endif
  ulonglong  next_insert_id,last_insert_id,current_insert_id;
  ha_rows select_limit,offset_limit,default_select_limit,cuted_fields,
          max_join_size;
  ulong query_id,version, inactive_timeout,options,thread_id;
  long  dbug_thread_id;
  pthread_t  real_id;
  uint	current_tablenr,tmp_table,cond_count,col_access,query_length;
  uint  server_status;
  char	     scramble[9];
  bool	     set_query_id,locked,count_cuted_fields,some_tables_deleted;
  bool	     no_errors, allow_sum_func, password, fatal_error;
  bool	     query_start_used,last_insert_id_used,insert_id_used,user_time;
  bool	     volatile killed,bootstrap;
  bool	     system_thread,in_lock_tables,global_read_lock;
  bool       query_error;
  THD();
  ~THD();
  bool store_globals();
  inline time_t query_start() { query_start_used=1; return start_time; }
  inline void	set_time()    { if (!user_time) time(&start_time); }
  inline void	set_time(time_t t) { start_time=t; user_time=1; }
  inline void	insert_id(ulonglong id)
  { last_insert_id=id; insert_id_used=1; }
  inline ulonglong insert_id(void)
  {
    if (!last_insert_id_used)
    {      
      last_insert_id_used=1;
      current_insert_id=last_insert_id;
    }
    return last_insert_id;
  }
};


class sql_exchange :public Sql_alloc
{
public:
  char *file_name;
  String *field_term,*enclosed,*line_term,*line_start,*escaped;
  bool opt_enclosed;
  bool dumpfile;
  uint skip_lines;
  sql_exchange(char *name,bool dumpfile_flag);
  ~sql_exchange() {}
};

#include "log_event.h"

/*
** This is used to get result from a select
*/

class select_result :public Sql_alloc {
protected:
  THD *thd;
public:
  select_result();
  virtual ~select_result() {};
  virtual int prepare(List<Item> &list) { return 0; }
  virtual bool send_fields(List<Item> &list,uint flag)=0;
  virtual bool send_data(List<Item> &items)=0;
  virtual void send_error(uint errcode,const char *err)=0;
  virtual bool send_eof()=0;
  virtual void abort() {}
};


class select_send :public select_result {
public:
  select_send() {}
  bool send_fields(List<Item> &list,uint flag);
  bool send_data(List<Item> &items);
  void send_error(uint errcode,const char *err);
  bool send_eof();
};


class select_export :public select_result {
  sql_exchange *exchange;
  File file;
  IO_CACHE cache;
  ha_rows row_count;
  uint field_term_length;
  int field_sep_char,escape_char,line_sep_char;
  bool fixed_row_size;
public:
  select_export(sql_exchange *ex) :exchange(ex),file(-1),row_count(0L) {}
  ~select_export();
  int prepare(List<Item> &list);
  bool send_fields(List<Item> &list,
		   uint flag) { return 0; }
  bool send_data(List<Item> &items);
  void send_error(uint errcode,const char *err);
  bool send_eof();
};

class select_dump :public select_result {
  sql_exchange *exchange;
  File file;
  IO_CACHE cache;
  ha_rows row_count;
  char path[FN_REFLEN];
public:
  select_dump(sql_exchange *ex) :exchange(ex),file(-1),row_count(0L)
  { path[0]=0; }
  ~select_dump();
  int prepare(List<Item> &list);
  bool send_fields(List<Item> &list,
		   uint flag) { return 0; }
  bool send_data(List<Item> &items);
  void send_error(uint errcode,const char *err);
  bool send_eof();
};


class select_insert :public select_result {
 protected:
  TABLE *table;
  List<Item> *fields;
  uint save_time_stamp;
  ulonglong last_insert_id;
  COPY_INFO info;

public:
  select_insert(TABLE *table_par,List<Item> *fields_par,enum_duplicates duplic)
    :table(table_par),fields(fields_par), save_time_stamp(0),last_insert_id(0)
    {
      bzero((char*) &info,sizeof(info));
      info.handle_duplicates=duplic;
    }
  ~select_insert();
  int prepare(List<Item> &list);
  bool send_fields(List<Item> &list,
		   uint flag) { return 0; }
  bool send_data(List<Item> &items);
  void send_error(uint errcode,const char *err);
  bool send_eof();
};

class select_create: public select_insert {
  ORDER *group;
  const char *db;
  const char *name;
  List<create_field> *extra_fields;
  List<Key> *keys;
  HA_CREATE_INFO *create_info;
  MYSQL_LOCK *lock;
  Field **field;
public:
  select_create (const char *db_name, const char *table_name,
		 HA_CREATE_INFO *create_info_par,
		 List<create_field> &fields_par,
		 List<Key> &keys_par,
		 List<Item> &select_fields,enum_duplicates duplic)
    :select_insert (NULL, &select_fields, duplic), db(db_name),
    name(table_name), extra_fields(&fields_par),keys(&keys_par),
    create_info(create_info_par),
    lock(0)
    {}
  int prepare(List<Item> &list);
  bool send_data(List<Item> &values);
  bool send_eof();
  void abort();
};

/* Structs used when sorting */

typedef struct st_sort_field {
  Field *field;				/* Field to sort */
  Item	*item;				/* Item if not sorting fields */
  uint	 length;			/* Length of sort field */
  my_bool reverse;			/* if descending sort */
  Item_result result_type;		/* Type of item */
} SORT_FIELD;


typedef struct st_sort_buffer {
  uint index;					/* 0 or 1 */
  uint sort_orders;
  uint change_pos;				/* If sort-fields changed */
  char **buff;
  SORT_FIELD *sortorder;
} SORT_BUFFER;


/* Structure for db & table in sql_yacc */

class Table_ident :public Sql_alloc {
 public:
  LEX_STRING db;
  LEX_STRING table;
  inline Table_ident(LEX_STRING db_arg,LEX_STRING table_arg,bool force)
    :table(table_arg)
  {
    if (!force && (current_thd->client_capabilities & CLIENT_NO_SCHEMA))
      db.str=0;
    else
      db= db_arg;
  }
  inline Table_ident(LEX_STRING table_arg) :table(table_arg) {db.str=0;}
  inline void change_db(char *db_name)
  { db.str= db_name; db.length=strlen(db_name); }
};

// this is needed for user_vars hash
class user_var_entry
{
 public:
  LEX_STRING name;
  char *value;
  ulong length;
  Item_result type;
};

