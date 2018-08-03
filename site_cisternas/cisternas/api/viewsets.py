from cisternas.models import Cisterna, Medicao
from .serializers import CisternaSerializer, MedicaoSerializer
from rest_framework import viewsets



class CisternaViewSet(viewsets.ModelViewSet):
	"""docstring for CisternaViewSet"""

	# list, create, retrieve, update, delete
	queryset = Cisterna.objects.all()
	serializer_class = CisternaSerializer
	# def __init__(self, arg):
	# 	super(CisternaViewSet, self).__init__()
	# 	self.arg = arg

class MedicaoViewSet(viewsets.ModelViewSet):
	"""docstring for MedicaoViewSet"""

	queryset = Medicao.objects.all()
	serializer_class = MedicaoSerializer