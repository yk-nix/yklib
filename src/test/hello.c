
#include "jni.h"
#include "hello.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JNIEXPORT void JNICALL Java_hello_print_1hello_1world(JNIEnv *env, jobject obj) {
	printf("hello, world!\n");
}

JNIEXPORT jint JNICALL Java_hello_yk_1get_1max(JNIEnv *evn, jobject obj, jint v1, jint v2) {
	return v1 > v2 ? v1 : v2;
}

JNIEXPORT jstring JNICALL Java_hello_yk_1add_1prefix(JNIEnv *env, jobject obj, jstring s) {
	jstring ret = NULL;
	const char *orig = (*env)->GetStringUTFChars(env, s, 0);
	if(orig == NULL)
		return NULL;
	int len = strlen(orig);
	char buf[128];
	memset(buf, 0, sizeof(buf));
	if(len + 3 < sizeof(buf)) {
		sprintf(buf, "yk_%s", orig);
		ret = (*env)->NewStringUTF(env, buf);
	}
	(*env)->ReleaseStringUTFChars(env, s, orig);
	return ret;
}
