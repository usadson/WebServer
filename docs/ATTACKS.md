# Attacks
The internet is constantly under attack. Lot's of bots trying CVE's, default settings, weak security etc. Therefore this software is built to protect from attacks.

## Slow loris
This attack is quite hard to patch. Slow loris works by creating lot's of connections and slowly sending (parts) of the request, intentionally. This looks like clients with slow internet connections are connection, and you don't want to mitigate those false positives.

## Security Defenses
The following modules are built into this software:
- Maximum requests per connection
- Maximum connection lifetime.
- Maximum header field name and value contents
- Maximum method length
- Maximum request-target (path) length
- Maximum whitespaces repetition
