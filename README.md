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
* Locations ---> JP (+someone?)
	* Obtenir la bonne url (ex: domain.com/blog/tech/ with location /blog/ --> root: /wordpress/ )
* Add try-catch where needed
* Clean code
    * Isolate function for building response page based on generated html-body --> JP
* Tests

## Done

* POST ---> JP
    * [DONE] File upload landing page
* Acces a une URL d'un dossier ---> JP
    * [DONE] File-listing : verifier au prealable l'existence du dossier + divers fix
	* [DONE] Si pas de slash a la fin de l'URL
	    * Status '301 Moved Permanently' + Redirection http-header --> 'Location: URL_ENDING_WITH_SLASH'