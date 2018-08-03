from django.shortcuts import render, get_object_or_404

from .models import Cisterna

# pubnub library
# veja https://www.pubnub.com/docs/python/pubnub-python-sdk#how_to_get_it
from pubnub.pnconfiguration import PNConfiguration
from pubnub.pubnub import PubNub

# Create your views here.

def cisternas_list(request):
	# return render(request, 'cisternas/cisternas-list.html')
	try:
		# cisterna = get_object_or_404(Cisterna, slug=cisterna_slug)
		cisternas_list = Cisterna.objects.all()
	except Cisterna.DoesNotExist as dne:
		raise Http404("Cisterna não existe / nenhuma cisterna encontrada: {}".format(dne))
	return render(request, 'cisternas/cisternas-list.html', {'cisternas_list': cisternas_list})

def cisternas_detail(request, cisterna_slug):
	# fetch the correct cisterna
	print("slug: %s" % cisterna_slug)
	try:
		cisterna = get_object_or_404(Cisterna, slug=cisterna_slug)
		# https://www.pubnub.com/docs/python/best-practices-playbook#optimal-nearest-point-of-presence-event-handler
		# https://www.pubnub.com/docs/blocks/tutorials/xhr
		# http --debug --form POST http://127.0.0.1:8000/api/medicoes/ cisterna="http://127.0.0.1:8000/api/cisternas/3/" time_stamp="2018-08-03T17:32:17" litros="65"

		# https://www.pubnub.com/docs/python/best-practices-playbook#optimal-offline-backup-with-storage-and-playback
		# https://www.pubnub.com/docs/python/pubnub-python-sdk#how_to_get_it
		# pugbnub_test()
	except Cisterna.DoesNotExist as dne:
		raise Http404("Cisterna não existe {}".format(dne))
	return render(request, 'cisternas/cisternas-detail.html', {'cisterna': cisterna, 'medicoes_list': cisterna.medicoes.all()})
	# return render(request, 'cisternas/cisternas-detail.html', {'cisterna': cisterna})

def pugbnub_test():
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