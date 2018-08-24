from django.shortcuts import render, get_object_or_404

from .models import Cisterna, Medicao

# pubnub library
# veja https://www.pubnub.com/docs/python/pubnub-python-sdk#how_to_get_it
from pubnub.pnconfiguration import PNConfiguration
from pubnub.pubnub import PubNub



# framework REST (TODO remover os imports abaixo)
# from django.http import HttpResponse, JsonResponse
from rest_framework import status
from rest_framework.views import APIView
from rest_framework.response import Response
from django.views.decorators.csrf import csrf_exempt
from rest_framework.renderers import JSONRenderer
from rest_framework.parsers import JSONParser
# from snippets.models import Snippet
from rest_framework.decorators import api_view
from django.http import Http404
from cisternas.api.serializers import CisternaSerializer

# autenticação
from rest_framework import permissions

class CisternaList(APIView):
	"""
	List all cisternas, or create a cisterna.\n
	Listar todas as cisternas, ou criar uma nova.
	"""
	# permission_classes = (permissions.IsAuthenticated)

	def get(self, request, format=None):
		cisternas = Cisterna.objects.all()
		serializer = CisternaSerializer(cisternas, many=True)
		return Response(serializer.data)

	def post(self, request, format=None):
		serializer = CisternaSerializer(data=request.data)
		if serializer.is_valid():
			serializer.save()
			return Response(serializer.data, status=status.HTTP_201_CREATED)
		return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


class CisternaDetail(APIView):
	"""
	Retrieve, update or deleta a cisterna instance
	"""

	permission_classes = (permissions.IsAuthenticated)

	def get_object(self, pk):
		try:
			return Cisterna.objects.get(pk=pk)
		except Cisterna.DoesNotExist as dne:
			raise Http404("Cisterna não existe / nenhuma cisterna encontrada: {}".format(dne))

	def get(self, request, pk, format=None):
		cisterna = self.get_object(pk)
		serializer = CisternaSerializer(cisterna)
		return Response(serializer.data)

	def put(self, request, pk, format=None):
		cisterna = self.get_object(pk)
		serializer = CisternaSerializer(cisterna, data=request.data)
		if serializer.is_valid():
			serializer.save()
			return Response(serializer.data)
		return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

	def delete(self, request, pk, format=None):
		cisterna = self.get_object(pk)
		cisterna.delete()
		return Response(status=status.HTTP_204_NO_CONTENT)

from django.contrib.auth.models import User
from rest_framework import generics
from cisternas.api.serializers import UserSerializer

class UserList(generics.ListAPIView):
    queryset = User.objects.all()
    serializer_class = UserSerializer


class UserDetail(generics.RetrieveAPIView):
    queryset = User.objects.all()
    serializer_class = UserSerializer


# @csrf_exempt
# @api_view(['GET', 'POST'])
# def cis_list(request, format=None):
#     """
#     List all code snippets, or create a new snippet.
#     """
#     # Teste com:
# 	# http GET http://127.0.0.1:8000/teste_api/
# 	# http POST http://127.0.0.1:8000/teste_api/ nome="Cisterna_teste3" escola="Escola_teste2"
# 	# http --debug --json POST http://127.0.0.1:8000/teste_api/ nome="Cisterna_teste3" escola="Escola_teste2"
# 	# 
#     if request.method == 'GET':
#         cisterna = Cisterna.objects.all()
#         serializer = CisternaSerializer(cisterna, many=True)
#         # return JsonResponse(serializer.data, safe=False)
#         return Response(serializer.data)

#     elif request.method == 'POST':
#         # data = JSONParser().parse(request)
#         # serializer = CisternaSerializer(data=data)
#         serializer = CisternaSerializer(data=request.data)
#         if serializer.is_valid():
#             serializer.save()
#             # return JsonResponse(serializer.data, status=201)
#             return Response(serializer.data, status=status.HTTP_201_CREATED)
#         # return JsonResponse(serializer.errors, status=400)
#         return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

# @csrf_exempt # essa anotation é para o rest_framework ignorar a proteção de acesso e permitir PUT e DELETE
# @api_view(['GET', 'PUT', 'DELETE'])	
# def cis_detail(request, pk, format=None):
# 	''' Retrieve, update or deleta '''
# 	# Teste com:
# 	# http GET http://127.0.0.1:8000/teste_api/3/
# 	# http PUT http://127.0.0.1:8000/teste_api/3/ nome="Cisteeehernna" escola="Escola"
# 	# http DELETE http://127.0.0.1:8000/teste_api/3/
# 	try:
# 		cisterna = Cisterna.objects.get(pk=pk)
# 	except Exception as e:
# 		# return HttpResponse(status=404)
# 		return Response(status=status.HTTP_404_NOT_FOUND)

# 	if request.method == 'GET':
# 		serializer = CisternaSerializer(cisterna)
# 		# return JsonResponse(serializer.data)
# 		return Response(serializer.data)
# 	elif request.method == 'PUT':
# 		# data = JSONParser().parse(request)
# 		# serializer = CisternaSerializer(cisterna, data=data)
# 		serializer = CisternaSerializer(cisterna, data=request.data)
# 		if serializer.is_valid():
# 			serializer.save()
# 			# return JsonResponse(serializer.data)
# 			return Response(serializer.data)
# 		# return JsonResponse(serializer.errors, status=400)
# 		return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
# 	elif request.method == 'DELETE':
# 		cisterna.delete()
# 		# return HttpResponse(status=204)
# 		return Response(status=status.HTTP_204_NO_CONTENT)


# Create your views here.

def cisternas_list(request):
	# return render(request, 'cisternas/cisternas-list.html')
	try:
		# cisterna = get_object_or_404(Cisterna, slug=cisterna_slug)
		cisternas_list = Cisterna.objects.all()
	except Cisterna.DoesNotExist as dne:
		raise Http404("Cisterna não existe / nenhuma cisterna encontrada: {}".format(dne))
	return render(request, 'cisternas/cisternas-list.html', {'cisternas_list': cisternas_list, 'medicao_count': Medicao.objects.all().count})

def cisternas_detail(request, cisterna_slug):
	# fetch the correct cisterna
	# print("slug: %s" % cisterna_slug)
	try:
		cisterna = get_object_or_404(Cisterna, slug=cisterna_slug)
		# https://www.pubnub.com/docs/python/best-practices-playbook#optimal-nearest-point-of-presence-event-handler
		# https://www.pubnub.com/docs/blocks/tutorials/xhr
		# http --debug --form GET http://127.0.0.1:8000/api/medicoes/ pk="13"
		# http --debug --form POST http://127.0.0.1:8000/api/medicoes/ cisterna="http://127.0.0.1:8000/api/cisternas/3/" time_stamp="2018-08-03T17:32:17" litros="65"
		# http --debug --form DELETE http://127.0.0.1:8000/api/medicoes/ pk="13" não funcionou

		# no servidor pythonanywhere
		# http --debug GET http://marcelosc.pythonanywhere.com/api/medicoes/1/
		# http --debug --form POST http://marcelosc.pythonanywhere.com/api/medicoes/ cisterna="http://marcelosc.pythonanywhere.com/api/cisternas/1/" time_stamp="2018-08-08T13:41:17" litros="65"

		# https://www.pubnub.com/docs/python/best-practices-playbook#optimal-offline-backup-with-storage-and-playback
		# https://www.pubnub.com/docs/python/pubnub-python-sdk#how_to_get_it

		# json
		# {"eon":{"time_stamp":"2018-08-08T20:55:31", "litros":40}}
		# {"comando":"ativa"}
		# {"cisterna":1,"time_stamp":"2018-08-08T20:55:31","eon":{"litros":40}}

		# 2018-08-23T17:40:43

		# pugbnub_test()

	except Cisterna.DoesNotExist as dne:
		raise Http404("Cisterna não existe {}".format(dne))
	return render(request, 'cisternas/cisternas-detail.html', {'cisterna': cisterna, 'medicoes_list': cisterna.medicoes.all()})
	# return render(request, 'cisternas/cisternas-detail.html', {'cisterna': cisterna})

def pugbnub_test():
	''' 
	Subscreve no canal de dados da cisterna, solicita e imprime as últimas
	100 mensagens do histórico do canal 
	'''
	print("initializing pubnub")
	# initializing pubnub

	pnconfig = PNConfiguration()
	pnconfig.subscribe_key = "sub-c-6e52b6c2-8e6b-11e8-b7a4-ce74daf54d52"
	# pnconfig.publish_key = "pub-c-6ce39655-3209-4cea-acc9-543751fe4e55"
	pnconfig.ssl = False

	pubnub = PubNub(pnconfig)

	# subscribe
	pubnub.subscribe().channels('dados_cisterna').execute()

	# retrieve messsages from histrory
	# https://www.pubnub.com/docs/python/api-reference-storage-and-playback#history
	envelope = pubnub.history().channel("dados_cisterna").count(100).sync()
	print("envelope: ")
	print(envelope)
	print("str(envelope): ")
	print(str(envelope))
	print("envelope status")
	print(envelope.status)
	print(type(envelope))
	print(type(envelope.result))
	print(type(envelope.result.messages))
	# print(envelope.messages)
	for message in envelope.result.messages:
		print("message: ")
		print(message.entry)
		# 
		# print(message.timetoken)
	# unsubscribe
	pubnub.unsubscribe().channels("dados_cisterna").execute()
	# pass