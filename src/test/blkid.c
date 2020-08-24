#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <blkid/blkid.h>

#if 0
struct list_head {
	struct list_head *next, *prev;
};
int basic_test(int argc, char *argv[]) {
	blkid_cache cache = NULL;
	int ret;
	ret = blkid_get_cache(&cache, NULL);
	if(ret < 0) {
		fprintf(stderr, "error: parse cache file\n");
		return 1;
	}
	blkid_dev_iterate iter = blkid_dev_iterate_begin(cache);
	if(iter == NULL)
		goto _err0;
	blkid_dev dev;
	while(!blkid_dev_next(iter, &dev)) {
		const char *devname = blkid_dev_devname(dev);
		if(devname) {
			blkid_probe pr = blkid_new_probe_from_filename(devname);
			if(pr) {
				dev_t devno = blkid_probe_get_devno(pr);
				printf("%s  %d:%d  %u %u %d %u\n", devname, major(devno), minor(devno),
					blkid_probe_get_size(pr), blkid_probe_get_offset(pr),
					blkid_probe_get_sectorsize(pr), blkid_probe_get_sectors(pr));
				blkid_free_probe(pr);
			}
			
		}
		/*
		printf("bid_name: %s\n", dev->bid_name);
		printf("bid_type: %s\n", dev->bid_type);
		printf("bid_pri: %d\n", dev->bid_pri);
		printf("bid_label: %d\n", dev->bid_label);
		printf("-----------\n");
		*/
	}
	ret = blkid_probe_all(cache);
	if(ret < 0) {
		fprintf(stderr, "error probing devices\n");
	}
_err0:
	blkid_put_cache(cache);
	return ret;
}
#endif

int partition_probe_test(int argc, char *argv[]) {
	blkid_probe pr;
	blkid_partlist ls;
	int nparts, i, rc = 1;
	if(argc < 2)
		return 1;
	char *devname = argv[1];
	pr = blkid_new_probe_from_filename(devname);
	if(!pr) {
		printf("error: create probe handle for %s: %m\n", devname);
		goto err0;
	}
printf("secotrs: %llu\n", blkid_probe_get_sectors(pr));
printf("sector-size: %d\n", blkid_probe_get_sectorsize(pr));
dev_t devno = blkid_probe_get_devno(pr);
printf("major: %d   minor: %d\n", major(devno), minor(devno));
printf("offset: %llu  size: %llu\n", blkid_probe_get_offset(pr), blkid_probe_get_size(pr));
	ls = blkid_probe_get_partitions(pr);
	if(!ls) {
		printf("error: failed to get partitions: %m\n");
		goto err1;
	}
	nparts = blkid_partlist_numof_partitions(ls);
	printf("number of partitions: %d\n", nparts);
	for(i = 0; i < nparts; i++) {
		blkid_partition par = blkid_partlist_get_partition(ls, i);
		if(par) {
			printf("#%d  %llu  %llu  %#x\n", 
				blkid_partition_get_partno(par),
				blkid_partition_get_start(par),
				blkid_partition_get_size(par),
				blkid_partition_get_type(par));
		}
	}
	rc = 0;
err1:
	blkid_free_probe(pr);
err0:
	return rc;
}

int main(int argc, char *argv[]) {
	return partition_probe_test(argc, argv);
}
