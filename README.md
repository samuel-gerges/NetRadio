Auteurs
=======

Gerges Samuel
Jami Adam
Mrassi Yacine

Architecture et fichiers
========================

Notre projet est composé des fichiers `.c` suivants et leurs headers :

`aux.c` : fichier contenant des fonctions auxiliaires (lecture de fichiers, formatage...).

`linked_list.c` : fichier contenant des implémentations de listes chaînées.

`diffuseur.c` : contenant une implémentation générique des diffuseurs, à exécuter avec un fichier de configuration.

`gestionnaire.c` : contenant une implémentation générique des gestionnaires.

`client.c` : contenant une implémentation d'un client qui va dans un premier temps se connecter à un gestionnaire pour lui demander sa liste de diffuseurs et par la suite demander à l'utilisateur d'entrer les coordonnées du diffuseur auquel il souhaite se connecter.

`client2.c` : fait la même chose que `client.c` mais sans demander la liste du gestionnaire.

Notre projet contient également un fichier java `Client.java` qui reçoit les messages en UDP du diffuseur (mais seulement utilisé avec le fichier `diff2_config`).

Comment compiler le projet, exécuter et utiliser le programme ?
===============================================================

Un Makefile est fourni, ainsi pour compiler il suffit de taper `make` là où se trouvent les fichiers. Sont alors générés 3 exécutables `gestio`, `diff`, `client` et `client2`.

Pour les utiliser :

- `./gestio`, sans argument.

- `./diff config_file`, avec `config_file` un fichier de configuration de diffuseur présent dans le dossier `config_files`. Exemple d'utilisation : `./diff config_files/diff1_config`.

- `./client config_file`, avec `config_file` un fichier de configuration de client présent dans le dossier `config_files`. Exemple d'utilisation : `./client config_files/client1_config`.

- `./client2 config_file`, avec `config_file` un fichier de configuration de client présent dans le dossier `config_files`. Exemple d'utilisation : `./client2 config_files/client1_config`.

Voici un scénario d'utilisation possible :

- lancer un gestionnaire
- lancer un diffuseur
- lancer un autre diffuseur
- lancer `client`
- lancer `client2`

Pour le client en C, il vous suffit de faire `javac Client.java` puis `java Client`, en ayant au préalable lancé le diffuseur avec `diff2_config`.
