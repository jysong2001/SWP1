from cgi import parse_qs
from template import *

def application(environ, start_response):
    d = parse_qs(environ['QUERY_STRING'])
    a = d.get('a', [''])[0]
    b = d.get('b', [''])[0]
    if '' not in [a, b]:
        a, b = int(a), int(b)
        result = """
        <html>
            <body>
                <form action="">
                    Sum = %d, Multi = %d
                </form>
            </body>
        </html>
        """%(a+b,a*b)
        response_body = html+result
        start_response('200 OK', [
            ('Content-Type', 'text/html'),
            ('Content-Length', str(len(response_body)))
            ])
    else:
        response_body = html
        start_response('200 OK', [
            ('Content-Type', 'text/html'),
            ('Content-Length', str(len(response_body)))
            ])
    return [response_body]
