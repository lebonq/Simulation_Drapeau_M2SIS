# Simulation de drapeau
### Via un système masse-ressort

## Librairies

Cette simulation fonctionne uniquement via LIBGX3D, elle doit être installée si vous voulez compiler.

## Compilation

Sous linux simple make produit un exécutable `drapeau_lebonq`

Non testé sous Windows et MacOS

## Utilisation

Le programme se lance avec la commande `./drapeau_lebonq`. Il ne prend aucun paraemètre.

## Paramètre de simulation

### Horizontal

La constante de raideur k et la viscositée mu sont modifiables via l'interface. Elle controle directement le maillage geometrique du drapeau.

Les facteurs alpha et beta controlent respectivement les valeurs de k et z pour le maillage de courbure et de torsion.

### Vertical

Fa : frequence d'affichage

Fe : frequence d'echantillonage

Wind : permet de controler la force du vent

Slider sans nom est le nombre de liaison rompu lors de l'initialisation. (cela permet d'avoir un drapeau abime des le debut)

Seuil : le seuil de force auquel les liaisons sont rompues.

### Taille du drapeau et du maillage de collision



## Elements implementes

### Maillages

Les trois types de maillages ont ete implémenté. Le maillage géométrique, de courbure et de torsion.
Chacun est stocker dans son propre tableau pour une gestion plus simple des liens.

### Gravité 

La gravité est un ensemble de lien qui est applique sur chaque masse. ce lien est une force constante.

### Vent

A l'instar de la gravité le vent est aussi un ensemble de lien constant. Le vecteur de la force est modulé aléatoirement (voir ligne xx) pour donner du mouvement au drapeau.

### Dechirure

### Auto-collision

## Point d'amélioration 



## Vidéo

https://youtu.be/8T_YyeyOHtE

Lien vers la vidéo sur Youtube, cette video a ete prise abvant la mise en place de l'auto-collision et reflete mieux la qualite de la simulation.


### Images

![images1](images/images1.png)
![images2](images/images2.png)
![images3](images/images3.png)
![images4](images/images4.png)
![images5](images/images5.png)
![images6](images/images6.png)
