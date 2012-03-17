#include <stdlib.h>
#include <stdio.h>

#include <libxml/nanohttp.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>


static const char *iciba_api = "http://dict-co.iciba.com/api/dictionary.php";

#define XML_BUF_MAX_SIZE		(1024 * 64)
static char xml_buf[XML_BUF_MAX_SIZE];
static char answer_buf[XML_BUF_MAX_SIZE];

int fd_iciba_init()
{
	xmlNanoHTTPInit();
	xmlXPathInit();

	return 0;
}

void fd_iciba_deinit()
{
}

static int parseSent(xmlNodePtr sent_node, char *buf, int sent_idx)
{
	xmlNodePtr tmp_node = sent_node->children;
	xmlChar *text;
	int off = 0;

	if (sent_idx == 0)
		off += sprintf(buf, "\nSentences:\n");

	while (tmp_node) {
		if (strcmp(tmp_node->name, "orig") == 0) {
			text = xmlNodeGetContent(tmp_node);
			off += sprintf(buf + off, "%d. %s\n", sent_idx, text);
			free(text);
		} else if (strcmp(tmp_node->name, "pron") == 0) {
			text = xmlNodeGetContent(tmp_node);
			off += sprintf(buf + off, "[%s]\n", text);
			free(text);
		} else if (strcmp(tmp_node->name, "trans") == 0) {
			text = xmlNodeGetContent(tmp_node);
			off += sprintf(buf + off, "%s\n", text);
			free(text);
		}
		tmp_node = tmp_node->next;
	}

	return off;
}

char *fd_iciba_lookup(const char *words)
{
	void *http_ctx;
	char *content_type;
	int rc;
	char *answer = NULL;
	char url[256];
	int i;

	sprintf(url, "%s?w=%s", iciba_api, words);
	http_ctx = xmlNanoHTTPOpen(url, &content_type);
	if (!http_ctx)
		return NULL;

	rc = xmlNanoHTTPRead(http_ctx, xml_buf, XML_BUF_MAX_SIZE);
	if (rc > 0) {
		/* reset */
		sprintf(answer_buf, "Not found no iciba!\n");

		/* incase xml parse failed, fallback to raw text */
		answer = xml_buf;

		xmlDocPtr doc = xmlParseMemory(xml_buf, rc);
		if (doc) {
			xmlXPathContextPtr xpath_ctx = xmlXPathNewContext(doc);
			xmlXPathObjectPtr xpath_obj = xmlXPathEval("/dict", xpath_ctx);
			xmlNodeSetPtr nodeset = xpath_obj->nodesetval;
			if (nodeset && nodeset->nodeNr) {
				xmlNodePtr dict_node = nodeset->nodeTab[0];

				xmlNodePtr tmp_node = dict_node->children;
				int off = 0, sent_idx = 0;
				xmlChar *text;
				while (tmp_node) {
					if (strcmp(tmp_node->name, "key") == 0) {
						text = xmlNodeGetContent(tmp_node);
						off += sprintf(answer_buf + off, "%s\n", text);
						free(text);
					} else if (strcmp(tmp_node->name, "ps") == 0) {
						text = xmlNodeGetContent(tmp_node);
						off += sprintf(answer_buf + off, "%s", text);
						free(text);
						tmp_node = tmp_node->next;
						if (tmp_node) {
							if (strcmp(tmp_node->name, "pron") == 0) {
								text = xmlNodeGetContent(tmp_node);
								off += sprintf(answer_buf + off, " [%s]\n", text);
								free(text);
							} else {
								continue;
							}
						} else {
							break;
						}
					} else if (strcmp(tmp_node->name, "pos") == 0) {
						text = xmlNodeGetContent(tmp_node);
						off += sprintf(answer_buf + off, "%s ", text);
						free(text);
					} else if (strcmp(tmp_node->name, "acceptation") == 0) {
						text = xmlNodeGetContent(tmp_node);
						off += sprintf(answer_buf + off, "%s\n", text);
						free(text);
					} else if (strcmp(tmp_node->name, "sent") == 0) {
						off += parseSent(tmp_node, answer_buf + off, sent_idx);
						sent_idx++;
					}
					tmp_node = tmp_node->next;
				}
				answer = answer_buf;
			}
			xmlXPathFreeContext(xpath_ctx);
		}
	}

	free(content_type);
	xmlNanoHTTPClose(http_ctx);

	return answer;
}

