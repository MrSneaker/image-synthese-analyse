import cv2
import numpy as np
import matplotlib.pyplot as plt
import re
from collections import Counter


imagePuzzleCouleurRGB=cv2.imread('image_puzzle.jpeg')
imagePuzzleCouleurHSV=cv2.cvtColor(imagePuzzleCouleurRGB,cv2.COLOR_BGR2HSV)
imagePuzzleCouleurHSVGauss=cv2.GaussianBlur(imagePuzzleCouleurHSV,(5,5),0)
imagePuzzleNoirEtBlanc=cv2.imread('image_puzzle.jpeg',0)

longueurImage=imagePuzzleNoirEtBlanc.shape[1]
largeurImage=imagePuzzleNoirEtBlanc.shape[0]

    
def plageDeCouleurZoneInterressante(image,haut,bas,gauche,droite):
    tabCouleurZoneInterressante=[[],[],[]]
    for i in range (int(haut),int(bas)):
        for j in range (int(gauche),int(droite)):
            tabCouleurZoneInterressante[0].append(image[i][j][0])
            tabCouleurZoneInterressante[1].append(image[i][j][1])
            tabCouleurZoneInterressante[2].append(image[i][j][2])
    
    
    #on recherche la couleur qui apparait le plus pour chaque composante
    tabCouleurdominante=[]
    
    #on recherche la couleur qui apparait le plus pour la composante H
    compteH=Counter(tabCouleurZoneInterressante[0])
    tabCouleurdominante.append(compteH.most_common(1)[0][0])

    #on recherche la couleur qui apparait le plus pour la composante S
    compteS=Counter(tabCouleurZoneInterressante[1])
    tabCouleurdominante.append(compteS.most_common(1)[0][0])

    #on recherche la couleur qui apparait le plus pour la composante V
    compteV=Counter(tabCouleurZoneInterressante[2])
    tabCouleurdominante.append(compteV.most_common(1)[0][0])
    
    return tabCouleurdominante




def plageDeCouleurZoneInterressante1(image,haut,bas,gauche,droite):
    tabCouleurZoneInterressante=[]
    for i in range (int(haut),int(bas)):
        for j in range (int(gauche),int(droite)):
            tabCouleurZoneInterressante.append(image[i][j])
            
    
    # Compter le nombre de fois que chaque couleur apparaît
    compteur = {}
    for couleur in tabCouleurZoneInterressante:
        couleur_str = str(couleur)
        if couleur_str in compteur:
            compteur[couleur_str] += 1
        else:
            compteur[couleur_str] = 1

    # Trier les couleurs par ordre décroissant de fréquence
    couleurs_triees = sorted(compteur.items(), key=lambda x: x[1], reverse=True)
    # # Afficher la couleur la plus fréquente
    couleur_plus_frequente = couleurs_triees[0][0]
    #j'ai couleurs_triees='[ 12 211 201]' et je veux [12,211,201]
    couleur_plus_frequente = couleur_plus_frequente.replace('[', '')
    couleur_plus_frequente = couleur_plus_frequente.replace(']', '')
    #j'ai " 12 211 201" et je veux un tableau [12,211,201]
    couleur_plus_frequente = couleur_plus_frequente.split()
    couleur_plus_frequente = [int(i) for i in couleur_plus_frequente]
    print (couleur_plus_frequente)
    return couleur_plus_frequente


   
#20,int(largeurImage/2),int(longueurImage/15*13),int(longueurImage-20)
tabCouleurDominanteHSV=plageDeCouleurZoneInterressante1(imagePuzzleCouleurHSVGauss,0,largeurImage,0,longueurImage)



#parcours de chaque pixel de l'image : imagePuzzleCouleurHSV
#si le pixel est dans la plage de couleur de la zone interressante + ou - erreur
#on le remplace par un pixel blanc
#sinon on le remplace par un pixel noir
def HSVtoBW(image,tabCouleurDominanteHSV, erreur ):
    for i in range(image.shape[0]):
        for j in range(image.shape[1]):
            if(image[i][j][0]>=(tabCouleurDominanteHSV[0]-erreur) and image[i][j][0]<=(tabCouleurDominanteHSV[0]+erreur) and image[i][j][1]>(tabCouleurDominanteHSV[1]-erreur) and image[i][j][1]<(tabCouleurDominanteHSV[1]+erreur) and image[i][j][2]>(tabCouleurDominanteHSV[2]-erreur) and image[i][j][2]<(tabCouleurDominanteHSV[2]+erreur)):
                image[i][j]=[255,255,255]
            else:
                image[i][j]=[0,0,0]

        
HSVtoBW(imagePuzzleCouleurHSVGauss,tabCouleurDominanteHSV, 50)












#création d'un tableau 2D de niveau de gris vide 
# tabZone=[[(longueurImage-50)-(longueurImage/10*9)],[(largeurImage/2)-50]]

# for i in range (int(longueurImage/10*9),int(longueurImage-50)):
#     for j in range (50,int(largeurImage/2)):
#         #on rempli le tableau avec les niveaux de gris de l'image
#         tabZone[i][j]=imagePuzzleNoirEtBlanc[i][j]

# #on affiche le tableau
# print(tabZone)


    


#parcours de chaque pixel de l'image en couleur 
#parcours en hauteur 
# for i in range(imagePuzzleCouleur.shape[0]):
#     #parcours en largeur
#     for j in range(imagePuzzleCouleur.shape[1]):
#         #si le pixel est blanc
#         if(imagePuzzleNoirEtBlanc[i][j]==255):
#             #on le remplace par du noir
#             imagePuzzleCouleur[i][j]=[0,0,0]
#             imagePuzzleNoirEtBlanc[i][j]=0


#affichage de l'image puzzle tant que l'utilisateur n'a pas appuyé sur une touche
#cv2.imshow("Image Puzzle",imagePuzzleCouleur)

cv2.imshow("Image Puzzle Noir et Blanc",imagePuzzleCouleurHSVGauss)
cv2.waitKey(0)
cv2.destroyAllWindows()
