
# Webserv

## To do

* CGI
    * ...
* DELETE
    * Check file permissions
    * ...
* Droits d'acces (ecriture)
    * [Upload] Retourner erreur 403 si le dossier dans dans lequel on essaye d'uploader un fichier est protege en ecriture
	* [Delete] Retourner une erreur si le fichier/dossier qu'on essaye de Delete est protege en ecriture
* Acces a une URL d'un dossier
    * File-listing : verifier au prealable l'existence du dossier
	* Si pas de slash a la fin de l'URL
	    * Erreur 301
		* Redirection header si pas de slash a la fin de l'url ('Location: URL_ENDING_WITH_SLASH')
* POST
    * Renvoyer une page (dans le cas d'une image uploadee, afficher l'image sur cette page)