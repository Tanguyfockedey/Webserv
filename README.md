
# Webserv

## To do ---> who takes care of it

* CGI ---> Mats
    * ...
* DELETE ---> Mats
    * Check file permissions
    * ...
* Droits d'acces (ecriture) ---> JP
    * [Upload] Retourner erreur 403 si le dossier dans dans lequel on essaye d'uploader un fichier est protege en ecriture
	* [Delete] Retourner une erreur si le fichier/dossier qu'on essaye de Delete est protege en ecriture
* Acces a une URL d'un dossier ---> JP
    * [DONE] File-listing : verifier au prealable l'existence du dossier + divers fix
	* [DONE] Si pas de slash a la fin de l'URL
	    * Status '301 Moved Permanently' + Redirection http-header --> 'Location: URL_ENDING_WITH_SLASH'
* Locations ---> JP (+someone?)
	* Obtenir la bonne url (ex: domain.com/blog/tech/ with location /blog/ --> root: /wordpress/ )
* POST ---> JP
    * Renvoyer une page (dans le cas d'une image uploadee, afficher l'image sur cette page)
* Clean code
* Tests