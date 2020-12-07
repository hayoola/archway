# Archway

A scalable caching reverse-proxy designed to be cloud-native.
It's inspired by [Varnish](https://github.com/varnishcache/varnish-cache), but thanks to the [drogon](https://github.com/an-tao/drogon) framework, it uses a non-blocking I/O architecture rather than thread pools.  
Let's look at a bare minimum config file:

```
confVersion: core/v1alpha1
declarations:
- backend:
    # My ancient (!) Apache server 
    name: baseOne
    kind: singleHost
    hosts:
    - address: http://10.100.1.243:80
routines:
- hostProgram:
    hosts: ['*.archway.hayoola.host']
    functions:
      - stage: receiveRequest
        instructions:
        - setBackend: baseOne
```

Here we've declared a simple backend, then instructed the proxy to match `*.archway.hayoola.host` domain name and subsequently run a `hostProgram`.  
Inside the hostProgram, we have a `function` registered to run at the `receiveRequest` stage of the request processing. This function simply set the backend here, but can be more complex.
<br/>  
<br/>  
  
**Note**: This project is under heavy development, and it is not ready for public deployment.