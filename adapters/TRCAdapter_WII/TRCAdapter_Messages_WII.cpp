#include "precompiled_trcadapter_WII.h"

#ifndef _ITF_TRCMESSAGES_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_Messages_WII.h"
#endif //_ITF_TRCMESSAGES_WII_H_

namespace ITF
{

char* TRCMessages_WII::ms_szDvdWrongDisc[MAX_LANGUAGE] = 		{	"%s\n‚ÌƒfƒBƒXƒN‚ğƒZƒbƒg‚µ‚Ä‚­‚¾‚³‚¢B",
"Please insert the\n%s disc.",
"Bitte schiebe die\n%s-Disc ein.",
"Veuillez insérer le disque\n%s.",
"Inserta el disco de\n%s.",
"Inserisci il disco di\n%s.",
"Voer de disk van\n%s in."
};

char* TRCMessages_WII::ms_szDvdNoDisc[MAX_LANGUAGE] = 		{	"%s\n‚ÌƒfƒBƒXƒN‚ğƒZƒbƒg‚µ‚Ä‚­‚¾‚³‚¢B",
"Please insert the\n%s disc.",
"Bitte schiebe die\n%s-Disc ein.",
"Veuillez insérer le disque\n%s.",
"Inserta el disco de\n%s.",
"Inserisci il disco di\n%s.",
"Voer de disk van\n%s in."
};

/*note: auto fatal messaging may display its own message*/
char* TRCMessages_WII::ms_szDvdFatal[MAX_LANGUAGE] = 			{	"ƒGƒ‰[‚ª”­¶‚µ‚Ü‚µ‚½B\nƒCƒWƒFƒNƒgƒ{ƒ^ƒ“‚ğ‰Ÿ‚µ‚ÄƒfƒBƒXƒN‚ğ\næ‚èo‚µ‚Ä‚©‚çA–{‘Ì‚Ì“dŒ¹‚ğOFF‚É‚µ‚ÄA\n–{‘Ì‚Ìæˆµà–¾‘‚Ìw¦‚É]‚Á‚Ä‚­‚¾‚³‚¢B", 
"An error has occurred.\nPress the EJECT Button, remove the\ndisc, and turn off the power to the console.\nPlease read the Wii Operations Manual\n for further instructions. ",
"Ein Fehler ist aufgetreten.\nDrücke den Ausgabeknopf, entnimm die Disc\nund schalte die Wii-Konsole aus. Bitte lies die\nWii-Bedienungsanleitung, um weitere\nInformationen zu erhalten.",
"Une erreur est survenue.\nAppuyez sur le bouton EJECT, retirez\nle disque et éteignez la console.\nVeuillez vous référer au mode\nd'emploi Wii pour plus de détails.",
"Se ha producido un error.\nPulsa el Botón EJECT, extrae el disco y\napaga la consola. Consulta el manual de\ninstrucciones de la consola Wii para\nobtener más información.",
"Si è verificato un errore.\nPremi il pulsante EJECT, estrai il\ndisco e spegni la console. Per maggiori informazioni, consulta il\nmanuale di istruzioni della console Wii.",
"Er is een fout opgetreden. Druk op de\nEJECT-knop, verwijder de disk en zet\nhet Wii-systeem uit. Lees de\nWii-handleiding voor meer informatie."
};


char* TRCMessages_WII::ms_szDvdRetry[MAX_LANGUAGE] = 			{"ƒfƒBƒXƒN‚ğ“Ç‚ß‚Ü‚¹‚ñ‚Å‚µ‚½B\n‚­‚í‚µ‚­‚ÍWii–{‘Ì‚Ìæˆµà–¾‘‚ğ‚¨“Ç‚İ‚­‚¾‚³‚¢B",
"The disc could not be read.\nRefer to the Wii Operations Manual \nfor details. ",
"Diese Disc kann nicht gelesen werden.\nBitte lies die Wii-Bedienungsanleitung,\num weitere Informationen zu erhalten.",
"Impossible de lire le disque.\nVeuillez vous référer au mode d'emploi\nWii pour plus de détails.",
"No se puede leer el disco.\nConsulta el manual de instrucciones de\nla consola Wii para obtener más información.",
"Impossibile leggere il disco.\nPer maggiori informazioni, consulta il\nmanuale di istruzioni della console Wii.",
"De disk kan niet worden gelezen. Lees\nde Wii-handleiding voor meer\ninformatie."
};

/*
obsolete 
*/
char* TRCMessages_WII::ms_szDvdMotorOff[MAX_LANGUAGE] =		{"ƒQ[ƒ€‚ğ‘±‚¯‚éê‡‚ÍƒRƒ“ƒgƒ[ƒ‰‚Ì\nƒ{ƒ^ƒ“‚ğ‰Ÿ‚µ‚Ä‚­‚¾‚³‚¢B", 
"Press any button on the Wii Remote\nto continue.",
"Betätige einen beliebigen Knopf der\nWii-Fernbedienung, um fortzufahren.",
"Appuyez sur n'importe quel bouton\nsur la télécommande Wii pour continuer.",
"Pulsa cualquier botón del\nmando de Wii para continuar.",
"Premi un pulsante sul\ntelecomando Wii per proseguire.",
"Druk op ? om door te gaan"
};


/*
obsolete  ?
*/
char* TRCMessages_WII::ms_szDvdReadingDisc[MAX_LANGUAGE] =	{"ƒ[ƒh‚µ‚Ä‚¢‚Ü‚·",
"Reading disc%s",
"Lese Disc%s",
"Lecture du disque%s",
"Leyendo el disco%s",
"Lettura disco%s",
"Disk lezen%s"
};

char* TRCMessages_WII::ms_szNandSystemCorrupt[MAX_LANGUAGE] =	{"Wii–{‘Ì•Û‘¶ƒƒ‚ƒŠ‚ª‰ó‚ê‚Ü‚µ‚½B\n‚­‚í‚µ‚­‚ÍWii–{‘Ì‚Ìæˆµà–¾‘‚ğ‚¨“Ç‚İ‚­‚¾‚³‚¢B",
"The Wii System Memory has been damaged.\nRefer to the Wii Operations Manual for details.",
"Der Speicher der Wii-Konsole ist beschädigt.\nBitte lies die Wii-Bedienungsanleitung,\num weitere Informationen zu erhalten.",
"La mémoire de la console Wii est endommagée.\nVeuillez vous référer au mode d'emploi Wii\npour plus de détails.",
"La memoria de la consola Wii está dañada.\nConsulta el manual de instrucciones de la consola Wii para obtener más información.",
"La memoria della console Wii è\ndanneggiata. Per maggiori informazioni,\nconsulta il manuale di istruzioni della\nconsole Wii.",
"Het interne geheugen van het\nWii-systeem is beschadigd. Lees de\nWii-handleiding voor meer informatie."
};

char* TRCMessages_WII::ms_szNandContentFilesCorrupt[MAX_LANGUAGE] =	{" %s\n‚ÌƒVƒXƒeƒ€ƒtƒ@ƒCƒ‹‚ª‰ó‚ê‚Ä‚¢‚Ü‚·B",
"The system file for %s is corrupted.\nAfter deleting %s from the Channels management screen, please download %s from the Wii Shop Channel again.\nIf this message still appears, visit support.nintendo.com.",
"Die Systemdatei von %s ist fehlerhaft.\nBitte lösche zuerst %s aus dem Kanäle-Verwaltungsbildschirm und lade dann erneut %s im Wii-Shop-Kanal herunter.\nBitte besuche support.nintendo.com, falls diese Meldung weiterhin angezeigt wird.",
"Le fichier système %s est corrompu.\nVeuillez effacer %s à l'aide de l'écran de gestion des chaînes, puis téléchargez de nouveau %s via la chaîne boutique Wii.\nSi ce message s'affiche toujours, visitez le site support.nintendo.com.",
"El archivo de sistema de %s está dañado.\nBorra %s de la pantalla de gestión de los canales y descarga %s del Canal Tienda Wii de nuevo.\nSi no se soluciona el problema, visita el sitio web support.nintendo.com.",
"Il file di sistema di %s è danneggiato.\nDopo aver eliminato %s dallo schermo gestione canali, scarica nuovamente %s dal Canale Wii Shop.\nSe riappare questo messaggio, visita il sito Internet support.nintendo.com",
"Het systeembestand van %s is beschadigd.\nWis %s in het kanalenbeheerscherm en download %s opnieuw in het Wii-winkelkanaal.\nGa naar support.nintendo.com als deze boodschap blijft verschijnen."
};

char* TRCMessages_WII::ms_szNandUnknownError[MAX_LANGUAGE] =	{"Wii–{‘Ì•Û‘¶ƒƒ‚ƒŠ‚Ì‘‚«‚İ/“Ç‚İo‚µ’†‚É\nƒGƒ‰[‚ª”­¶‚µ‚Ü‚µ‚½B\n‚­‚í‚µ‚­‚ÍWii–{‘Ì‚Ìæˆµà–¾‘‚ğ‚¨“Ç‚İ‚­‚¾‚³‚¢B",
"An error occurred during the process of reading \nfrom or writing to the Wii System Memory.\n Please refer to the Wii Operations Manual for details.",
"Beim Zugriff auf den Speicher der Wii-Konsole \nist ein Fehler aufgetreten.\n Bitte lies die Wii-Bedienungsanleitung,\n um weitere Informationen zu erhalten.",
"Une erreur est survenue avec la mémoire de la console Wii\n pendant le processus de lecture ou d'écriture.\n Veuillez vous référer au mode d'emploi Wii\n pour plus de détails.",
"Se ha producido un error durante la lectura o escritura\n de la memoria de la consola Wii.\n Consulta el manual de instrucciones de la consola Wii\n para obtener más información.",
"Si è verificato un errore durante la lettura o la modifica\n dei dati all'interno della memoria della console Wii.\n Per maggiori informazioni,\n consulta il manuale di istruzioni della console Wii.",
"Er is een fout opgetreden tijdens het lezen of beschrijven\n van het interne geheugen van het Wii-systeem.\n Lees de Wii-handleiding voor meer informatie."
};

u32 TRCMessages_WII::getLanguageIndex()
{
    switch (SCGetLanguage())
    {
    case SC_LANG_JAPANESE            :  return TRC_LANG_JAPANESE;
    case SC_LANG_ENGLISH             :  return TRC_LANG_ENGLISH;
    case SC_LANG_GERMAN              :  return TRC_LANG_GERMAN;
    case SC_LANG_FRENCH              :  return TRC_LANG_FRENCH;
    case SC_LANG_SPANISH             :  return TRC_LANG_SPANISH;
    case SC_LANG_ITALIAN             :  return TRC_LANG_ITALIAN;
    case SC_LANG_DUTCH               :  return TRC_LANG_DUTCH;

    //not supported yet
    case SC_LANG_SIMP_CHINESE:
    case SC_LANG_TRAD_CHINESE:
    case SC_LANG_KOREAN:
    default:
        // default language
        return TRC_LANG_ENGLISH; 
    }   
}

} //namespace ITF
