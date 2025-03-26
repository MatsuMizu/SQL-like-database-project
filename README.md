# SQL-like Database Project

**A custom database implementation with SQL-like syntax using AVL trees and hashing**

## Project Goals
- Study **AVL-trees** and other data structures
- Practice **string manipulation** at scale
- Understand the complexity behind **SQL implementations**
- Includes client-server version (available in `.zip`)

## Database Structure
**Student records with:**
- Name (AVL-tree indexed)
- Phone (Hashed by last K digits + AVL-tree)
- Group (0-999, with separate substructures)

### Search Optimization
- **Group-known queries**: Uses 1 of 1000 substructures
- **Name/Phone-only queries**: Searches main structure
- **All structures** operate with references only

## Supported SQL-like Commands
- insert (name, phone, group);
- select fields [where condition] [order by fields];
- delete [where conditions];

## Command examples:
- insert (Student, 1234567, 208);
- select group, name where phone = 1234567 and name = Student;
- select * where phone >= 1234567 and name like St% order by group;
- select name, phone where group = 208 and phone <> 1234567;
- select * where name = Student or phone = 1234567;
- select name where name not like St% and phone = 1234567 and group = 208 order by name;
- select * order by name, phone, group;
- delete where name = Student;
- delete where phone = 1234567 and group = 208 and name not like Student; 


## How to run a program:
This program is supposed to be ran in your console (in cmd as you run all other programs with console input).
Open config.txt to configure phone hashing, change students info in students_list.txt.
Using cmd go to the directory with all the files, type 'make' command.
Type '.\a.out student_list.txt' to start SQL-like mode.
Type in commands you want, SELECT output will be also shown in command line.
Press ctrl+C to quit.
