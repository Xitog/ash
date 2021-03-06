# Introduction

Ash est né des limitations frustrantes de Lua :

* La fusion des listes et des tableaux
    * L'impossibilité de distinguer entre une clé inexistante et une clé pointant vers nul :
    
      ```a = {'bbb'} ; a['bbb'] = nil comme a['ccc'] = nil !```
      
    * La possibilité de faire des trous :
    
      ```a[0] = 'aaa' mais aussi a['5'] = 'ccc'```
      
* L'utilisation de ~= au lieu du plus standard != pour l'opérateur de différence,
* L'absence d'opérateurs arithmétiques combinés comme +=, -=, /=, *=, %=, **=,
* L'utilisation du très long elseif plutôt que elif,
* Le fait qu'une variable soit globale par défaut : il faut mettre local sinon,
* L'impossibilité de définir des constantes,
* L'absence de tests sur le nombre de paramètres passés à une fonction,
* L'impossibilité de définir des contraintes ou des hints de types,
* L'absence de séparateur de ligne,
* La fusion des entiers et des réels en un seul type.
* in ne s'utilise pas directement sur une table, il faut utiliser pairs ou ipairs
* Une clé chaîne dans une table doit être entourée de crochets

    ```['key'] = val```

Lua a néanmoins des forces que j'ai reprises pour Ash :

* L'élégance des commentaires avec --
* L'élégance de l'opérateur # pour avoir la longueur
* L'élégance d'utiliser directement un symbole comme clé dans les tables
* L'élégance d'utiliser = pour associer clés et valeurs dans une table
* L'élégance de ne pas avoir à définir avant son appel une fonction

# 1. Premiers pas avec Ash

Ash est un langage de programmation, un langage pour donner des ordres à un ordinateur. Ash est un langage interprété, c'est-à-dire qu'un interprétateur va lire du code Ash et donner les ordres correspondants à l'ordinateur. D'autres langages, comme C, sont compilés : le code C est directement traduit en ordres compréhensibles directement par l'ordinateur à l'aide d'un compilateur. Un programme C compilé n'a donc pas besoin d'un interpréteur pour être exécuté. D'autres langages, comme Python, Ruby ou Java, sont compilés dans un langage intermédiaire de bas niveau, qui n'est pas compréhensible directement par l'ordinateur. Le programme est ensuite exécuté dans une machine virtuelle, qui communique au véritable ordinateur les ordres correspondants. Ash dispose également d'un transpileur qui transforme le code Ash en code Python.

__Compilateur vers langage d'ordinateur__

* Entrée : code de haut niveau dans le langage L
* Action : traduit le code de haut niveau écrit dans le langage L en code compréhensible par l'ordinateur
* Sortie : code compréhensible par l'ordinateur

__Compilateur vers langage intermédiaire__

* Entrée : code de haut niveau dans le langage L
* Action : traduit le code de haut niveau écrit dans le langage L en un langage intermédiaire de plus bas niveau mais non compréhensible par l'ordinateur
* Sortie : code en langage intermédiaire

__Interpréteur__

* Entrée : code de haut niveau dans le langage L
* Action : lit le code de haut niveau écrit dans le langage L et donne les instructions correspondantes à l'ordinateur
* Sortie : selon le code

__VM__

* Entrée : code en langage intermédiaire
* Action : lit le code de bas niveau écrit dans le langage intermédiaire et donne les instructions correspondantes à l'ordinateur.
* Sortie : selon le code

__Transpiler__

* Entrée : code en langage de haut niveau L1
* Action : traduit le code dans un autre langage de haut niveau L2
* Sortie : code en langage de haut niveau L2

On peut écrire directement du code Ash dans l'interpréteur, ou dans un fichier texte, encodé en ascii ou utf8, et en passant celui-ci à l'interpréteur dans un second temps. Pour exécuter le fichier mon_script.ash, il suffit de faire :
python ash.py mon_script
Il n'y a pas besoin de repréciser l'extension du fichier, l'interpréteur sait qu'elle sera .ash et l'ajoute automatiquement, on peut néanmoins l'écrire.

Si on lance l'interpréteur sans aucun argument, une invite de commande s'affiche et on peut directement écrire dedans des instructions :

```lua
Ash 1.0 2011-2019
>
```

Pour commencer, nous allons suivre la tradition en affichant le message "Hello World" dans la console. Pour donner cette instruction à l'ordinateur, il suffit d'écrire dans l'interpréteur :

writeln est une fonction qui affiche ce qu'on lui donne en paramètre et finit par un saut de ligne et un retour à la première position.

Nous voulons ensuite personnaliser notre message en demandant son nom à l'utilisateur :

read est une fonction du module io qui va lire sur la console ce que tape l'utilisateur. Ici, on précise qu'on veut lire une chaîne de caractère (str pour String). On stocke cette valeur dans la variable name, puis on affiche à l'aide de writeln un message de bienvenue.

L'exemple 1 est constitué d'une seule instruction. L'exemple 2 est constituée de deux instructions. Un programme Ash est constitué d'une suite d'instructions et globalement, une instruction correspond à une ligne. Les sauts de ligne jouent un rôle important dans la syntaxe d'Ash. Si on veut mettre deux instructions sur la même ligne, il faut les séparer par un point-virgule. Ainsi l'exemple 2 est équivalent à l'exemple 3 :

L'interpréteur exécute les instructions Ash une par une immédiatement dans ce qu'on appelle une boucle REPL : read/lire - eval/interpréter - print/afficher - loop/boucler. Pour quitter cette boucle, il suffit de taper exit. Parfois, une instruction ne peut être exécutée seule et doit être complétée par une autre. Dans ce cas-là, l'interpréteur change son invite de commande en >>.

Pour exécuter plus d'un fichier, il faut précéder les autres par -m[odule] / -l[oad]. Ils seront exécuté dans l'ordre où ils sont donnés puis le script principal sera exécuté. 

```lua
python ash.py mon_script_principal -m mon_script1 -m mon_script2
```

Si on veut que l'exécution d'un script soit suivi d'une session interactive, comme lorsqu'on lance l'interpréteur sans fichier, il faut utiliser l'option -i[nteractive].

```lua
python ash.py mon_script -i
```

L’option -c[ommand] / -e[val] permet de donner directement du code à l’interpréteur.

```lua
python ash.py -c "name = io.read(str) ; writln(‘Hello’, name)”
```

On peut également lancer l'interpréteur avec l'option -v ou --version pour avoir sa version et il s'arrête immédiatement après l'avoir affiché dans la console.

On peut passer des arguments à un script donné à l’interpréteur. Il suffit de les mettre dans la ligne de commande :
python ash.py mon_script arg1 arg2
On les retrouve dans la liste Args (il s’agit d’une liste constante). On accède aux arguments à l’aide de la syntaxe Args[indice]. L’emplacement 0 contient le nom du script, l’emplacement 1 contient le premier argument, etc.

Dans l'interpréteur on peut appeler la fonction import pour exécuter un fichier depuis l'interpréteur.

```lua
> import("mon_script")
```

ou

```lua
> import "mon_script"
```

La seconde façon montre deux choses : la première, c'est que les parenthèses pour appeler une fonction sont optionnelles si l'expression n'est pas ambiguë.

Commentaire

Un commentaire permet d'ajouter des informations à du code. Un commentaire n'est jamais interprété. Un commentaire commence par deux tirets -- et tout ce qui suit jusqu’à la prochaine ligne sera considéré comme commenté.

```lua
-- Ceci est un commentaire
```

Un commentaire multiligne doit occuper une ligne entière. Il commence par un double égal ==. Toute la ligne et les suivantes sont considérées comme commentées. Cela ne s’arrête que quand un nouveau symbole de commentaire multiligne est rencontrée. Cette ligne sera alors la dernière à être commentée.

```lua
== Ceci est un commentaire multiligne
Le commentaire continue ici.
== Ceci est la dernière ligne du commentaire multiligne
```

Mots clés

Les 26 mots suivant, appelés mots clés, sont réservés et ne peuvent être utilisés comme noms de variable :

repeat

# 2. Variables et constantes

Une variable est une référence vers une valeur. Dans l'exemple 2 et 3, name est une variable qui référence la valeur entrée par l'utilisateur dans la console. La première utilisation d'une variable est appelée une déclaration. Lors de la déclaration, on peut préciser définir plusieurs paramètres qui ne pourront plus être changés par la suite.

On peut préciser un type et une valeur initiale référencée :

```lua
a : int = 5
```

Mais ce n'est pas obligatoire :

```lua
a = 5
```

La différence entre les deux déclarations est que dans la deuxième, a pourra être utilisée pour référence un entier, mais aussi un réel ou une chaîne par la suite. Son type est dynamique. Dans la première déclaration, il est spécifié que a ne pourra référencer que des entiers. Enfin on peut préciser que a ne peut référencer qu'une valeur précise en utilisant une majuscule comme première lettre du nom :

```lua
A : int = 5
A = 5
```

Les deux déclarations précédentes sont équivalentes, il n'est pas obligatoire de spécifier int car l'interpréteur déduira lui-même le type de A par rapport à la valeur à droite du signe égal.

Si on essaye d'accéder à la valeur référencée par une variable qui n'a jamais été déclarée, on obtient une erreur :

```lua
> writeln(x)
NameError: name 'x' is not defined
```

Si on affecte nil a une variable, cela signifie que la variable ne référence aucune valeur.

```lua
a = nil
```

Si on définit un type pour a, il faut spécifier un point d'interrogation à la fin du type pour autoriser l'affectation à nil :

```lua
> a : int = nil
NilError : variable 'a' is of type int, not int?
> a : int? = nil
```

Une variable déclarée dans un script a une portée globale. 

Un nom de variable ne doit pas commencer par un chiffre et peut contenir les 26 lettres de l'alphabet latin standard et des soulignés. Une variable ne peut avoir pour nom un mot clé. Ash fait la différence entre les minuscules et les majuscules, ainsi abc et ABC ne sont pas la même variable. De plus, nous l’avons vu, toute variable commençant par une majuscule est en fait une constante : elle référence une valeur et cette référence ne peut être changée.