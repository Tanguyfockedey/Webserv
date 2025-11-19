_config.get_location_path(_req.get_uri())

std::string common = common_path(path1, path2);

# Webserv

## To do ---> who takes care of it

* CGI ---> Mats
    * fix bug when file has no read permission
* DELETE ---> Mats
    * Handling scenarios leading to 204, 403, 409, 500 status codes (see schema)
* Protect POST of size 0 --> JP
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


* request.cpp --> converts "/" into "/index.html"  ONLY IF REQUEST METHOD == GET
* send 405 if POST URL is a directory (in other words, filename.empty())
* send 405 Method Not Allowed instead of 501 Not Implemented for HEAD method (to please tester)
* remove trailing whitespaces from requests

* created a directory called "directory" to pass a test ? 



