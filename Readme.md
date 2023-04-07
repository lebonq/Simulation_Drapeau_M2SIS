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

## Elements implementes

### Maillages

### Vent

### Dechirure

### Auto-collision

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
