from django.shortcuts import render

from django.http import HttpResponse

from .models import Question

def index(request):
	print("haduken")
	print(request)
	quest = Question.objects.all()
	texto = ", ".join([str(q) for q in quest])
	print(texto)
	# print(request.kwargs)
	# return HttpResponse("Hello, world. You're at the polls index")
	return HttpResponse(texto)

# Create your views here.
