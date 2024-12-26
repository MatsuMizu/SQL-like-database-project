# SQL-like database project

This repository is my project on creating a database that can be manipulated with using SQL-like commands.
The main goal was to study AVL-trees and other structures, practice working with lines and realize, how much effort was put in SQL creation.
There's also a client-server version implemented, all the required files are stored in corresponding .zip archive.

PROJECT DESCRIPTION AND USAGE:

The database itself consists of records of Students and has information about their Name, Group and Phone.
To optimize the speed of operating with data Names info is organized as AVL-tree and Phones are hashed depending on their last K numbers and are also kept as AVL-tree.
Also there is the same structure as described for each Group (0 - 999).
So, if you are searching for an object with a known group, the program uses one of 1000 substructures. Group number is defined in conditions.h
If only the Name/Phone is known, the search is done though the original structure, that's not divided by groups.
Command parsing is also implemented handly.
The records are stored in single instance, all the structures operate with links only.

This database model supports most common simplified SQL commands:
• insert (<name>, <phone>, <group>);
• select <fields needed or * sign> [where <condition>] [order by <fields>];
• delete [where <conditions>];
\>, <, =, <=, >= and != can be set as conditions, no symbols are required apart from mentioned in commands themselves.
... and ..., ... or ..., ... and ... and ..., ... or ... or ... are also supported.
To set conditions for Name use % and * signs:
_ - any symbol, % - any amount of any symbols, [n-m] - any symbol between letter n and letter m, [^n-m] - any symbol apart from n-m range.
To use any of these special symbols or \\ as a literal symbol put \ before it: \\[ is interpreted as "[".

Command examples:
• insert (Student, 1234567, 208);
• select group, name where phone = 1234567 and name = Student;
• select * where phone >= 1234567 and name like St% order by group;
• select name, phone where group = 208 and phone <> 1234567;
• select * where name = Student or phone = 1234567;
• select name where name not like St% and phone = 1234567 and group = 208 order by name;
• select * order by name, phone, group;
• delete where name = Student;
• delete where phone = 1234567 and group = 208 and name not like Student; 


HOW TO RUN A PROGRAM:
This program is supposed to be ran in your console (in cmd as you run all other programs with console input).
Open config.txt to configure phone hashing, change students info in students_list.txt.
Using cmd go to the directory with all the files, type 'make' command.
Type '.\a.out student_list.txt' to start SQL-like mode.
Type in commands you want, SELECT output will be also shown in command line.
Press ctrl+C to quit.
