# Virtual Addressing

### cpuid: physical and virtual pages

```
1-GB large page support                = true
```

CPUID also gives me insight into what will be more commonplace in future based
on the instructions my CPU & system doesn't currently support:

Seems we'll start having more machines with 5-level paging in future:

```
LA57: 57-bit addrs & 5-level paging      = false
```

```
# support for 4MB memory pages

PSE: page size extensions              = true
# support for huge pages
PSE-36: page size extension            = true

# support for more than 4GB of RAM
PAE: physical address extensions       = true

PAT: page attribute table              = true
```
