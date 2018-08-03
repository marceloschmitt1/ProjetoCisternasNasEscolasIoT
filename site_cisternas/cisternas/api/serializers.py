from rest_framework import serializers
from cisternas.models import Cisterna
from cisternas.models import Medicao


class CisternaSerializer(serializers.HyperlinkedModelSerializer):
	"""docstring for CisternaSerializer"""

	class Meta(object):
		"""docstring for Meta"""
		model = Cisterna
		fields = ('nome', 'escola', 'status', 'status_bomba', 'litros', 'ultima_medicao')


class MedicaoSerializer(serializers.HyperlinkedModelSerializer):
	"""docstring for MedicaoSerializer"""

	class Meta(object):
		"""docstring for Meta"""
		model = Medicao
		fields = ('cisterna', 'time_stamp', 'litros')

