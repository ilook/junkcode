/*
 * testHTML.c : a small tester program for HTML input.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#include "system.h"
#include <libxml/xmlversion.h>

#ifdef LIBXML_HTML_ENABLED

#include <stdarg.h>

#include <rpmio.h>
#include <rpmdir.h>
#include <rpmdav.h>
#include <rpmcb.h>
#include <argv.h>
#include <popt.h>

#include <libxml/xmlmemory.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/debugXML.h>
#include <libxml/xmlerror.h>
#include <libxml/globals.h>

#ifdef LIBXML_DEBUG_ENABLED
static int _debug = 0;
#endif
static int copy = 0;
static int sax = 0;
static int repeat = 0;
static int noout = 0;
#ifdef LIBXML_PUSH_ENABLED
static int push = 0;
#endif /* LIBXML_PUSH_ENABLED */
static char *encoding = NULL;
static int options = 0;

static xmlSAXHandler emptySAXHandlerStruct = {
    NULL, /* internalSubset */
    NULL, /* isStandalone */
    NULL, /* hasInternalSubset */
    NULL, /* hasExternalSubset */
    NULL, /* resolveEntity */
    NULL, /* getEntity */
    NULL, /* entityDecl */
    NULL, /* notationDecl */
    NULL, /* attributeDecl */
    NULL, /* elementDecl */
    NULL, /* unparsedEntityDecl */
    NULL, /* setDocumentLocator */
    NULL, /* startDocument */
    NULL, /* endDocument */
    NULL, /* startElement */
    NULL, /* endElement */
    NULL, /* reference */
    NULL, /* characters */
    NULL, /* ignorableWhitespace */
    NULL, /* processingInstruction */
    NULL, /* comment */
    NULL, /* xmlParserWarning */
    NULL, /* xmlParserError */
    NULL, /* xmlParserFatal */
    NULL, /* getParameterEntity */
    NULL, /* cdataBlock */
    NULL, /* externalSubset */
    1,    /* initialized */
    NULL, /* private */
    NULL, /* startElementNsSAX2Func */
    NULL, /* endElementNsSAX2Func */
    NULL  /* xmlStructuredErrorFunc */
};

static xmlSAXHandlerPtr emptySAXHandler = &emptySAXHandlerStruct;
extern xmlSAXHandlerPtr debugSAXHandler;

/************************************************************************
 *									*
 *				Debug Handlers				*
 *									*
 ************************************************************************/

/**
 * isStandaloneDebug:
 * @ctxt:  An XML parser context
 *
 * Is this document tagged standalone ?
 *
 * Returns 1 if true
 */
static int
isStandaloneDebug(void *ctx ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.isStandalone()\n");
    return(0);
}

/**
 * hasInternalSubsetDebug:
 * @ctxt:  An XML parser context
 *
 * Does this document has an internal subset
 *
 * Returns 1 if true
 */
static int
hasInternalSubsetDebug(void *ctx ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.hasInternalSubset()\n");
    return(0);
}

/**
 * hasExternalSubsetDebug:
 * @ctxt:  An XML parser context
 *
 * Does this document has an external subset
 *
 * Returns 1 if true
 */
static int
hasExternalSubsetDebug(void *ctx ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.hasExternalSubset()\n");
    return(0);
}

/**
 * hasInternalSubsetDebug:
 * @ctxt:  An XML parser context
 *
 * Does this document has an internal subset
 */
static void
internalSubsetDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name,
	       const xmlChar *ExternalID, const xmlChar *SystemID)
{
    fprintf(stdout, "SAX.internalSubset(%s,", name);
    if (ExternalID == NULL)
	fprintf(stdout, " ,");
    else
	fprintf(stdout, " %s,", ExternalID);
    if (SystemID == NULL)
	fprintf(stdout, " )\n");
    else
	fprintf(stdout, " %s)\n", SystemID);
}

/**
 * resolveEntityDebug:
 * @ctxt:  An XML parser context
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * Special entity resolver, better left to the parser, it has
 * more context than the application layer.
 * The default behaviour is to NOT resolve the entities, in that case
 * the ENTITY_REF nodes are built in the structure (and the parameter
 * values).
 *
 * Returns the xmlParserInputPtr if inlined or NULL for DOM behaviour.
 */
static xmlParserInputPtr
resolveEntityDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *publicId, const xmlChar *systemId)
{
    /* xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx; */

    
    fprintf(stdout, "SAX.resolveEntity(");
    if (publicId != NULL)
	fprintf(stdout, "%s", (char *)publicId);
    else
	fprintf(stdout, " ");
    if (systemId != NULL)
	fprintf(stdout, ", %s)\n", (char *)systemId);
    else
	fprintf(stdout, ", )\n");
/*********
    if (systemId != NULL) {
        return(xmlNewInputFromFile(ctxt, (char *) systemId));
    }
 *********/
    return(NULL);
}

/**
 * getEntityDebug:
 * @ctxt:  An XML parser context
 * @name: The entity name
 *
 * Get an entity by name
 *
 * Returns the xmlParserInputPtr if inlined or NULL for DOM behaviour.
 */
static xmlEntityPtr
getEntityDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    fprintf(stdout, "SAX.getEntity(%s)\n", name);
    return(NULL);
}

/**
 * getParameterEntityDebug:
 * @ctxt:  An XML parser context
 * @name: The entity name
 *
 * Get a parameter entity by name
 *
 * Returns the xmlParserInputPtr
 */
static xmlEntityPtr
getParameterEntityDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    fprintf(stdout, "SAX.getParameterEntity(%s)\n", name);
    return(NULL);
}


/**
 * entityDeclDebug:
 * @ctxt:  An XML parser context
 * @name:  the entity name 
 * @type:  the entity type 
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @content: the entity value (without processing).
 *
 * An entity definition has been parsed
 */
static void
entityDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name, int type,
          const xmlChar *publicId, const xmlChar *systemId, xmlChar *content)
{
    fprintf(stdout, "SAX.entityDecl(%s, %d, %s, %s, %s)\n",
            name, type, publicId, systemId, content);
}

/**
 * attributeDeclDebug:
 * @ctxt:  An XML parser context
 * @name:  the attribute name 
 * @type:  the attribute type 
 *
 * An attribute definition has been parsed
 */
static void
attributeDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *elem, const xmlChar *name,
              int type, int def, const xmlChar *defaultValue,
	      xmlEnumerationPtr tree ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.attributeDecl(%s, %s, %d, %d, %s, ...)\n",
            elem, name, type, def, defaultValue);
}

/**
 * elementDeclDebug:
 * @ctxt:  An XML parser context
 * @name:  the element name 
 * @type:  the element type 
 * @content: the element value (without processing).
 *
 * An element definition has been parsed
 */
static void
elementDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name, int type,
	    xmlElementContentPtr content ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.elementDecl(%s, %d, ...)\n",
            name, type);
}

/**
 * notationDeclDebug:
 * @ctxt:  An XML parser context
 * @name: The name of the notation
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * What to do when a notation declaration has been parsed.
 */
static void
notationDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name,
	     const xmlChar *publicId, const xmlChar *systemId)
{
    fprintf(stdout, "SAX.notationDecl(%s, %s, %s)\n",
            (char *) name, (char *) publicId, (char *) systemId);
}

/**
 * unparsedEntityDeclDebug:
 * @ctxt:  An XML parser context
 * @name: The name of the entity
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @notationName: the name of the notation
 *
 * What to do when an unparsed entity declaration is parsed
 */
static void
unparsedEntityDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name,
		   const xmlChar *publicId, const xmlChar *systemId,
		   const xmlChar *notationName)
{
    fprintf(stdout, "SAX.unparsedEntityDecl(%s, %s, %s, %s)\n",
            (char *) name, (char *) publicId, (char *) systemId,
	    (char *) notationName);
}

/**
 * setDocumentLocatorDebug:
 * @ctxt:  An XML parser context
 * @loc: A SAX Locator
 *
 * Receive the document locator at startup, actually xmlDefaultSAXLocator
 * Everything is available on the context, so this is useless in our case.
 */
static void
setDocumentLocatorDebug(void *ctx ATTRIBUTE_UNUSED, xmlSAXLocatorPtr loc ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.setDocumentLocator()\n");
}

/**
 * startDocumentDebug:
 * @ctxt:  An XML parser context
 *
 * called when the document start being processed.
 */
static void
startDocumentDebug(void *ctx ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.startDocument()\n");
}

/**
 * endDocumentDebug:
 * @ctxt:  An XML parser context
 *
 * called when the document end has been detected.
 */
static void
endDocumentDebug(void *ctx ATTRIBUTE_UNUSED)
{
    fprintf(stdout, "SAX.endDocument()\n");
}

/**
 * startElementDebug:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when an opening tag has been processed.
 */
static void
startElementDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name, const xmlChar **atts)
{
    int i;

    fprintf(stdout, "SAX.startElement(%s", (char *) name);
    if (atts != NULL) {
        for (i = 0;(atts[i] != NULL);i++) {
	    fprintf(stdout, ", %s", atts[i++]);
	    if (atts[i] != NULL) {
		unsigned char output[40];
		const unsigned char *att = atts[i];
		int outlen, attlen;
	        fprintf(stdout, "='");
		while ((attlen = strlen((char*)att)) > 0) {
		    outlen = sizeof output - 1;
		    htmlEncodeEntities(output, &outlen, att, &attlen, '\'');
		    output[outlen] = 0;
		    fprintf(stdout, "%s", (char *) output);
		    att += attlen;
		}
		fprintf(stdout, "'");
	    }
	}
    }
    fprintf(stdout, ")\n");
}

/**
 * endElementDebug:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void
endElementDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    fprintf(stdout, "SAX.endElement(%s)\n", (char *) name);
}

/**
 * charactersDebug:
 * @ctxt:  An XML parser context
 * @ch:  a xmlChar string
 * @len: the number of xmlChar
 *
 * receiving some chars from the parser.
 * Question: how much at a time ???
 */
static void
charactersDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *ch, int len)
{
    unsigned char output[40];
    int inlen = len, outlen = 30;

    htmlEncodeEntities(output, &outlen, ch, &inlen, 0);
    output[outlen] = 0;

    fprintf(stdout, "SAX.characters(%s, %d)\n", output, len);
}

/**
 * cdataDebug:
 * @ctxt:  An XML parser context
 * @ch:  a xmlChar string
 * @len: the number of xmlChar
 *
 * receiving some cdata chars from the parser.
 * Question: how much at a time ???
 */
static void
cdataDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *ch, int len)
{
    unsigned char output[40];
    int inlen = len, outlen = 30;

    htmlEncodeEntities(output, &outlen, ch, &inlen, 0);
    output[outlen] = 0;

    fprintf(stdout, "SAX.cdata(%s, %d)\n", output, len);
}

/**
 * referenceDebug:
 * @ctxt:  An XML parser context
 * @name:  The entity name
 *
 * called when an entity reference is detected. 
 */
static void
referenceDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    fprintf(stdout, "SAX.reference(%s)\n", name);
}

/**
 * ignorableWhitespaceDebug:
 * @ctxt:  An XML parser context
 * @ch:  a xmlChar string
 * @start: the first char in the string
 * @len: the number of xmlChar
 *
 * receiving some ignorable whitespaces from the parser.
 * Question: how much at a time ???
 */
static void
ignorableWhitespaceDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *ch, int len)
{
    char output[40];
    int i;

    for (i = 0;(i<len) && (i < 30);i++)
	output[i] = ch[i];
    output[i] = 0;

    fprintf(stdout, "SAX.ignorableWhitespace(%s, %d)\n", output, len);
}

/**
 * processingInstructionDebug:
 * @ctxt:  An XML parser context
 * @target:  the target name
 * @data: the PI data's
 * @len: the number of xmlChar
 *
 * A processing instruction has been parsed.
 */
static void
processingInstructionDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *target,
                      const xmlChar *data)
{
    fprintf(stdout, "SAX.processingInstruction(%s, %s)\n",
            (char *) target, (char *) data);
}

/**
 * commentDebug:
 * @ctxt:  An XML parser context
 * @value:  the comment content
 *
 * A comment has been parsed.
 */
static void
commentDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *value)
{
    fprintf(stdout, "SAX.comment(%s)\n", value);
}

/**
 * warningDebug:
 * @ctxt:  An XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 *
 * Display and format a warning messages, gives file, line, position and
 * extra parameters.
 */
static void XMLCDECL
warningDebug(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    fprintf(stdout, "SAX.warning: ");
    vfprintf(stdout, msg, args);
    va_end(args);
}

/**
 * errorDebug:
 * @ctxt:  An XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 *
 * Display and format a error messages, gives file, line, position and
 * extra parameters.
 */
static void XMLCDECL
errorDebug(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    fprintf(stdout, "SAX.error: ");
    vfprintf(stdout, msg, args);
    va_end(args);
}

/**
 * fatalErrorDebug:
 * @ctxt:  An XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 *
 * Display and format a fatalError messages, gives file, line, position and
 * extra parameters.
 */
static void XMLCDECL
fatalErrorDebug(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    fprintf(stdout, "SAX.fatalError: ");
    vfprintf(stdout, msg, args);
    va_end(args);
}

static xmlSAXHandler debugSAXHandlerStruct = {
    internalSubsetDebug,
    isStandaloneDebug,
    hasInternalSubsetDebug,
    hasExternalSubsetDebug,
    resolveEntityDebug,
    getEntityDebug,
    entityDeclDebug,
    notationDeclDebug,
    attributeDeclDebug,
    elementDeclDebug,
    unparsedEntityDeclDebug,
    setDocumentLocatorDebug,
    startDocumentDebug,
    endDocumentDebug,
    startElementDebug,
    endElementDebug,
    referenceDebug,
    charactersDebug,
    ignorableWhitespaceDebug,
    processingInstructionDebug,
    commentDebug,
    warningDebug,
    errorDebug,
    fatalErrorDebug,
    getParameterEntityDebug,
    cdataDebug,
    NULL,
    1,
    NULL,
    NULL,
    NULL,
    NULL
};

xmlSAXHandlerPtr debugSAXHandler = &debugSAXHandlerStruct;
/************************************************************************
 *									*
 *				Debug					*
 *									*
 ************************************************************************/
static htmlSAXHandler *pushSAXHandlerPtr = NULL;

static void XMLCDECL
pushQuiet(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...)
{
}

static void
parseSAXFile(const char *filename)
{
    htmlDocPtr doc = NULL;

    /*
     * Empty callbacks for checking
     */
#ifdef LIBXML_PUSH_ENABLED
    if (push) {
	FD_t f;

#if defined(_WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
	f = Fopen(filename, "rb%{?_rpmgio}");
#else
	f = Fopen(filename, "r%{?_rpmgio}");
#endif
	if (f != NULL) {
	    int res, size = 3;
	    char chars[4096];
	    htmlParserCtxtPtr ctxt;

	    /* if (repeat) */
		size = 4096;
	    res = Fread(chars, 1, 4, f);
	    if (res > 0) {
		ctxt = htmlCreatePushParserCtxt(emptySAXHandler, NULL,
			    chars, res, filename, XML_CHAR_ENCODING_NONE);
		while ((res = Fread(chars, 1, size, f)) > 0) {
		    htmlParseChunk(ctxt, chars, res, 0);
		}
		htmlParseChunk(ctxt, chars, 0, 1);
		doc = ctxt->myDoc;
		htmlFreeParserCtxt(ctxt);
	    }
	    if (doc != NULL) {
		fprintf(stdout, "htmlSAXParseFile returned non-NULL\n");
		xmlFreeDoc(doc);
	    }
	    Fclose(f);
	}
	if (!noout) {
#if defined(_WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
		f = Fopen(filename, "rb%{?_rpmgio}");
#else
		f = Fopen(filename, "r%{?_rpmgio}");
#endif
	    if (f != NULL) {
		int res, size = 3;
		char chars[4096];
		htmlParserCtxtPtr ctxt;

		/* if (repeat) */
		    size = 4096;
		res = Fread(chars, 1, 4, f);
		if (res > 0) {
		    ctxt = htmlCreatePushParserCtxt(debugSAXHandler, NULL,
				chars, res, filename, XML_CHAR_ENCODING_NONE);
		    while ((res = Fread(chars, 1, size, f)) > 0) {
			htmlParseChunk(ctxt, chars, res, 0);
		    }
		    htmlParseChunk(ctxt, chars, 0, 1);
		    doc = ctxt->myDoc;
		    htmlFreeParserCtxt(ctxt);
		}
		if (doc != NULL) {
		    fprintf(stdout, "htmlSAXParseFile returned non-NULL\n");
		    xmlFreeDoc(doc);
		}
		Fclose(f);
	    }
	}
    } else {	
#endif /* LIBXML_PUSH_ENABLED */
	doc = htmlSAXParseFile(filename, NULL, emptySAXHandler, NULL);
	if (doc != NULL) {
	    fprintf(stdout, "htmlSAXParseFile returned non-NULL\n");
	    xmlFreeDoc(doc);
	}

	if (!noout) {
	    /*
	     * Debug callback
	     */
	    doc = htmlSAXParseFile(filename, NULL, debugSAXHandler, NULL);
	    if (doc != NULL) {
		fprintf(stdout, "htmlSAXParseFile returned non-NULL\n");
		xmlFreeDoc(doc);
	    }
	}
#ifdef LIBXML_PUSH_ENABLED
    }
#endif /* LIBXML_PUSH_ENABLED */
}

static void
parseAndPrintFile(const char *filename)
{
    htmlDocPtr doc = NULL;

    /*
     * build an HTML tree from a string;
     */
#ifdef LIBXML_PUSH_ENABLED
    if (push) {
	FD_t f;

#if defined(_WIN32) || defined (__DJGPP__) && !defined (__CYGWIN__)
	f = Fopen(filename, "rb%{?_rpmgio}");
#else
	f = Fopen(filename, "r%{?_rpmgio}");
#endif
	if (f != NULL) {
	    int res, size = 3;
	    char chars[4096];
	    htmlParserCtxtPtr ctxt;

	    /* if (repeat) */
		size = 4096;
	    res = Fread(chars, 1, 4, f);
	    if (res > 0) {
		ctxt = htmlCreatePushParserCtxt(pushSAXHandlerPtr, NULL,
			    chars, res, filename, XML_CHAR_ENCODING_NONE);
		while ((res = Fread(chars, 1, size, f)) > 0) {
		    htmlParseChunk(ctxt, chars, res, 0);
		}
		htmlParseChunk(ctxt, chars, 0, 1);
		doc = ctxt->myDoc;
		htmlFreeParserCtxt(ctxt);
	    }
	    Fclose(f);
	}
    } else {	
	doc = htmlReadFile(filename, NULL, options);
    }
#else
	doc = htmlReadFile(filename,NULL,options);
#endif
    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"Could not parse %s\n", filename);
    }

#ifdef LIBXML_TREE_ENABLED
    /*
     * test intermediate copy if needed.
     */
    if (copy) {
        htmlDocPtr tmp;

        tmp = doc;
	doc = xmlCopyDoc(doc, 1);
	xmlFreeDoc(tmp);
    }
#endif

#ifdef LIBXML_OUTPUT_ENABLED
    /*
     * print it.
     */
    if (!noout) { 
#ifdef LIBXML_DEBUG_ENABLED
	if (!_debug) {
	    if (encoding)
		htmlSaveFileEnc("-", doc, encoding);
	    else
		htmlDocDump(stdout, doc);
	} else
	    xmlDebugDumpDocument(stdout, doc);
#else
	if (encoding)
	    htmlSaveFileEnc("-", doc, encoding);
	else
	    htmlDocDump(stdout, doc);
#endif
    }	
#endif /* LIBXML_OUTPUT_ENABLED */

    /*
     * free it.
     */
    xmlFreeDoc(doc);
}

static struct poptOption optionsTable[] = {
#ifdef LIBXML_DEBUG_ENABLED
 { "debug", '\0', POPT_ARG_NONE,		&_debug, 0,
	N_("dump a debug tree of the in-memory document"), NULL },
#endif
 { "copy", '\0', POPT_ARG_NONE,			&copy, 0,
	N_("used to test the internal copy implementation"), NULL },
  { "noout", '\0', POPT_ARG_NONE,		&noout, 0,
	N_("do not print the result"), NULL },
#ifdef LIBXML_PUSH_ENABLED
 { "push", '\0', POPT_ARG_NONE,			&push, 0,
	N_("use the push mode parser"), NULL },
#endif
 { "sax", '\0', POPT_ARG_NONE,			&sax, 0,
	N_("debug the sequence of SAX callbacks"), NULL },
 { "repeat", '\0', POPT_ARG_NONE,		&repeat, 0,
	N_("parse the file 100 times, for timing"), NULL },
 { "encoding", '\0', POPT_ARG_STRING,		&encoding, 0,
	N_("output in the given encoding"), N_("ENCODING") },

 { "avdebug", '\0', POPT_ARG_VAL|POPT_ARGFLAG_DOC_HIDDEN, &_av_debug, -1,
	N_("debug protocol data stream"), NULL},
 { "davdebug", '\0', POPT_ARG_VAL|POPT_ARGFLAG_DOC_HIDDEN, &_dav_debug, -1,
	N_("debug protocol data stream"), NULL},
 { "ftpdebug", '\0', POPT_ARG_VAL|POPT_ARGFLAG_DOC_HIDDEN, &_ftp_debug, -1,
	N_("debug protocol data stream"), NULL},
 { "rpmiodebug", '\0', POPT_ARG_VAL|POPT_ARGFLAG_DOC_HIDDEN, &_rpmio_debug, -1,
	N_("debug rpmio I/O"), NULL},
 { "urldebug", '\0', POPT_ARG_VAL|POPT_ARGFLAG_DOC_HIDDEN, &_url_debug, -1,
	N_("debug URL cache handling"), NULL},
 { "verbose", 'v', 0, 0, 'v',				NULL, NULL },

    POPT_AUTOHELP
    POPT_TABLEEND
};

int main(int argc, char *argv[])
{
    poptContext optCon = poptGetContext(argv[0], argc, (const char **)argv, optionsTable, 0);
    ARGV_t av = NULL;
    int rc;

    while ((rc = poptGetNextOpt(optCon)) > 0) {
	const char * optArg = poptGetOptArg(optCon);
	switch (rc) {
	case 'v':
	    rpmIncreaseVerbosity();
	    /*@switchbreak@*/ break;
	default:
            /*@switchbreak@*/ break;
	}
	optArg = _free(optArg);
    }

    if (_debug) {
	rpmIncreaseVerbosity();
	rpmIncreaseVerbosity();
_av_debug = -1;
_dav_debug = -1;
_ftp_debug = -1;
_url_debug = -1;
_rpmio_debug = -1;
    }

    pushSAXHandlerPtr = xmlMalloc(sizeof(*pushSAXHandlerPtr));
    memcpy(pushSAXHandlerPtr, &htmlDefaultSAXHandler, sizeof(*pushSAXHandlerPtr));
    if (!rpmIsDebug()) {
	pushSAXHandlerPtr->warning = pushQuiet;
	if (!rpmIsVerbose()) {
	    pushSAXHandlerPtr->error = pushQuiet;
	    if (!rpmIsNormal()) {
		pushSAXHandlerPtr->fatalError = pushQuiet;
	    }
	}
    }

    if ((av = poptGetArgs(optCon)) != NULL)
    while (*av) {
	int i = 100 * repeat + 1;
	while (i-- > 0)
	    if (sax)
		parseSAXFile(*av);
	    else   
		parseAndPrintFile(*av);
	av++;
    }

    xmlCleanupParser();
    xmlMemoryDump();

    xmlFree(pushSAXHandlerPtr);

/*@i@*/ urlFreeCache();

    optCon = poptFreeContext(optCon);


    return(0);
}
#else /* !LIBXML_HTML_ENABLED */
#include <stdio.h>
int main(int argc ATTRIBUTE_UNUSED, char **argv ATTRIBUTE_UNUSED) {
    printf("%s : HTML support not compiled in\n", argv[0]);
    return(0);
}
#endif
