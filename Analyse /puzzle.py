import cv2
import numpy as np
import matplotlib.pyplot as plt
import re
from collections import Counter


imageCouleurRGB=cv2.imread('exemple_feuilles.jpeg')

imageCouleurHSV=cv2.cvtColor(imageCouleurRGB,cv2.COLOR_BGR2HSV)
imageCouleurHSVGauss=cv2.GaussianBlur(imageCouleurHSV,(5,5),0)


longueurImage=imageCouleurHSVGauss.shape[1]
largeurImage=imageCouleurHSVGauss.shape[0]

#fonction permettant de définir la couleur la plus utilisée dans une zone interressante de l'image
def plageDeCouleurZoneInterressante(image,haut,bas,gauche,droite):
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
    # Afficher la couleur la plus fréquente
    couleur_plus_frequente = couleurs_triees[0][0]
    #Chaîne de caraceres à liste
    couleur_plus_frequente = couleur_plus_frequente.replace('[', '')
    couleur_plus_frequente = couleur_plus_frequente.replace(']', '')
    couleur_plus_frequente = couleur_plus_frequente.split()
    couleur_plus_frequente = [int(i) for i in couleur_plus_frequente]
    print ("la couleur dominante de la zone est : ")  
    print(couleur_plus_frequente)
    return couleur_plus_frequente


# Définition de la couleur dominante de l'image imageCouleurHSVGauss
CouleurDominanteHSV=plageDeCouleurZoneInterressante(imageCouleurHSVGauss,0,largeurImage,0,longueurImage)


#fonction permettant de transformer une image couleur en image noir et blanc en fonction de la couleur dominante de l'image
def HSVtoBW(image,CouleurDominanteHSV, erreur ):
    for i in range(image.shape[0]):
        for j in range(image.shape[1]):
            #si le pixel est dans la plage de couleur de la zone interressante + ou - erreur
            if(image[i][j][0]>=(CouleurDominanteHSV[0]-erreur) and image[i][j][0]<=(CouleurDominanteHSV[0]+erreur) and image[i][j][1]>(CouleurDominanteHSV[1]-erreur) and image[i][j][1]<(CouleurDominanteHSV[1]+erreur) and image[i][j][2]>(CouleurDominanteHSV[2]-erreur) and image[i][j][2]<(CouleurDominanteHSV[2]+erreur)):
                #on remplace le pixel par un pixel blanc
                image[i][j]=[255,255,255]
            else:
                #on remplace le pixel par un pixel noir
                image[i][j]=[0,0,0]

        
HSVtoBW(imageCouleurHSVGauss,CouleurDominanteHSV, 50)


cv2.imshow("Image Noir et Blanc",imageCouleurHSVGauss)
cv2.waitKey(0)
cv2.destroyAllWindows()
