# GaëlRack Fx — nouveau projet

Plugin d'effets vocaux JUCE/C++ avec interface sombre, accents rouge/vert/blanc, silhouette stylisée de Madagascar, paramètres Input/Mix/Output, EQ grave/médium/aigu, compression, saturation, Air et Afro.

## Formats

Le projet produit **VST3 et Standalone**. VST2 n'est pas activé dans cette version : JUCE requiert l'ancien SDK VST2 fourni par Steinberg et les droits/licences appropriés. Le SDK VST2 n'est pas redistribué ici.

## Compilation en ligne

1. Téléverser le contenu de ce dossier à la racine d'un dépôt GitHub, en conservant le dossier `.github/workflows`.
2. Ouvrir l'onglet Actions.
3. Lancer `Build GaelRackFx` (ou attendre le lancement après un commit).
4. Une fois terminé, télécharger l'artefact `GaelRackFx-Windows` en bas de la page du run.

Le workflow utilise le générateur Visual Studio 2022 préinstallé sur `windows-latest`. JUCE 8.0.6 est récupéré par CMake pendant la configuration, ce qui nécessite que le runner ait accès à Internet.

## Remarque licence JUCE

JUCE est proposé sous licence AGPLv3 ou sous licence commerciale. Vérifier les conditions applicables avant toute distribution propriétaire/commerciale : https://juce.com/get-juce/

## État

Le code est fourni comme nouvelle base de projet. Il doit être validé par une compilation réelle sur GitHub Actions; aucun résultat de compilation Windows n'est prétendu avant ce test.


## Compilation Windows (GitHub Actions)

Le workflow `.github/workflows/build.yml` compile le projet sur un runner Windows 2022 avec Visual Studio 2022. Dans GitHub, ouvre **Actions** → **Build GaelRack Fx** → **Run workflow**. Attends que toutes les étapes soient vertes, puis ouvre le run et télécharge l’artefact **GaelRackFx-Windows**. Il contient `GaelRackFx-VST3.zip`.

Après extraction, copie le dossier `GaelRackFx.vst3` dans `C:\Program Files\Common Files\VST3`, puis redémarre Studio One et vérifie le dossier VST3 dans ses paramètres. Si une étape échoue, ouvre cette étape et partage le texte complet de l’erreur. Une archive de sources n’est pas un plugin compilé : il faut que le workflow réussisse pour obtenir le VST3 installable.
