# Webserv

## To do ---> who takes care of it

* CGI ---> Mats
    * ...
* DELETE ---> Mats
    * Handling scenarios leading to 204, 403, 409, 500 status codes (see schema)

* Uploads ---> JP
	* Check if single-part upload is still used/relevant --> used if using NetCat utility (nc) instead of firefox for making the POST request, AND it needs to be fixed !
* Locations ---> JP (+someone?)
	* Obtenir la bonne url (ex: /blog/tech/janvier/ location /blog/tech/ --> root: /wordpress/ )

* Add try-catch where needed
* Clean code
    * Isolate function for building response page based on generated html-body --> JP
* Tests
* Clean repo
   * Delete unnecessary files (HTTP diagrams, ...)

## Done

* POST ---> JP
    * [DONE] File upload landing page
* Acces a une URL d'un dossier ---> JP
    * [DONE] File-listing : verifier au prealable l'existence du dossier + divers fix
	* [DONE] Si pas de slash a la fin de l'URL
	    * Status '301 Moved Permanently' + Redirection http-header --> 'Location: URL_ENDING_WITH_SLASH'
* Droits d'acces (ecriture) ---> JP
    * [DONE] [Upload] Retourner erreur 403 si le dossier dans lequel on essaye d'uploader un fichier est protege en ecriture
	* File permissions ---> JP
		* Try uploading a file with no read permission