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

## Other Defenses
- Privilege de-escalation
- <iframe> blocking
- Sending of XSS protection hints
- Disabling refer\[r\]er (privacy)
- CSP

## Future Defenses
The following ideas have come to my mind, but haven't been implemented (properly):
- Automatic IP blocking
- Automatic blocking of vulnerability scanners
- Automatic blocking of directory
- Caching to avoid I/O overload (ClientError & FileResolveStatus' FILE_SYSTEM_OVERLOAD)
- Full slowloris attack mitigation by read timeout total time checking
- Maximum message body length (this isn't actually needed because we skip that parsing either way)
- Maximum connections per IP
- Prioritizing connections from different IPs
- Service outage notifications
- R-U-Dead-Yet mitigation
