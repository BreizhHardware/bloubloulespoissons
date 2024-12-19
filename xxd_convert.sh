#!/bin/bash

# Vérifie si un argument de dossier est fourni
if [ $# -ne 1 ]; then
    echo "Usage: $0 <dossier>"
    exit 1
fi

# Vérifie si le dossier existe
DIR=$1
if [ ! -d "$DIR" ]; then
    echo "Erreur : Le dossier '$DIR' n'existe pas."
    exit 1
fi

# Parcourt tous les fichiers .png du dossier et sous-dossiers
find "$DIR" -type f -name "*.ttf" | while read -r file; do
    # Récupère le nom du fichier sans extension
    base_name=$(basename "$file" .ttf)
    # Définit le nom de sortie avec extension .h
    output_file="${file%/*}/${base_name}.h"
    # Exécute la commande xxd -i
    xxd -i "$file" > "$output_file"
    echo "Converti : $file -> $output_file"
done
