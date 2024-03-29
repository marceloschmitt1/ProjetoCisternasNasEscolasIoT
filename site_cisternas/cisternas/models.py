import datetime

from django.db import models

from django.utils import timezone
from django.utils.text import slugify


class Cisterna(models.Model):
	"""docstring for Cisterna"""

	nome = models.CharField(max_length=100)
	slug = models.SlugField(max_length=100, unique=True, blank=True)
	escola = models.CharField(max_length=160)
	status = models.BooleanField(default=False)
	status_bomba = models.BooleanField(default=False)
	# litros = models.FloatField(default=0.0)
	# ultima_medicao = models.DateTimeField('ultima medicao', blank=True, null=True)
	capacidade = models.IntegerField(default=600)
	# dono = models.ForeignKey('auth.User', related_name='cisternas', on_delete=models.SET_NULL, blank=True, null=True)


	def save(self, *args, **kwargs):
		self.slug = slugify(self.nome)
		super(Cisterna, self).save(*args, **kwargs)

	# def __init__(self, arg):
	# 	super(Cisterna, self).__init__()
	# 	self.arg = arg

	def __str__(self):
		return self.nome

	def esta_ativa(self):
		return self.ultima_medicao >= timezone.now() - datetime.timedelta(hours=1)


class Medicao(models.Model):
	"""docstring for Medicao"""

	cisterna = models.ForeignKey(Cisterna, on_delete=models.CASCADE, related_name='medicoes')
	time_stamp = models.DateTimeField('data da medicao')
	litros = models.FloatField()

	def __str__(self):
		return "{} litro(s) em {}".format(self.litros, self.time_stamp)

	class Meta:
		ordering = ('time_stamp',)
			

	# def __init__(self, arg):
	# 	super(Medicao, self).__init__()
	# 	self.arg = arg
		
