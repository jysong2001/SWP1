from cgi import parse_qs
from template import *

def application(environ, start_response):
    d = parse_qs(environ['QUERY_STRING'])
    a = d.get('a', [''])[0]
    b = d.get('b', [''])[0]
    try:
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
    except ValueError:
	result = """
	<html>
	    <body>
		<form action="">
		    invalid input
		</form>
	    </body>
	</html>
	"""
    finally:
        response_body = html+result
        start_response('200 OK', [
            ('Content-Type', 'text/html'),
            ('Content-Length', str(len(response_body)))
            ])
    return [response_body]
