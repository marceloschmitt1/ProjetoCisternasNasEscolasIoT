from rest_framework import serializers
from cisternas.models import Cisterna, Medicao


class CisternaSerializer(serializers.HyperlinkedModelSerializer):
	"""docstring for CisternaSerializer"""

	class Meta(object):
		"""docstring for Meta"""
		model = Cisterna
		# fields = ('nome', 'escola', 'status', 'status_bomba', 'litros', 'ultima_medicao', 'dono')
		# fields = ('nome', 'escola', 'status', 'status_bomba', 'litros', 'ultima_medicao', 'capacidade')
		fields = ('nome', 'escola', 'status', 'status_bomba', 'capacidade')


class MedicaoSerializer(serializers.HyperlinkedModelSerializer):
	"""docstring for MedicaoSerializer"""

	class Meta(object):
		"""docstring for Meta"""
		model = Medicao
		fields = ('cisterna', 'time_stamp', 'litros')


from django.contrib.auth.models import User

class UserSerializer(serializers.ModelSerializer):
    # cisternas = serializers.PrimaryKeyRelatedField(many=True, queryset=Cisternas.objects.all())

    class Meta:
        model = User
        fields = ('id', 'username')