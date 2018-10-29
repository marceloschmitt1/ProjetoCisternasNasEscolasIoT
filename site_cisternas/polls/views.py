# from django.shortcuts import render

from django.http import HttpResponse, HttpResponseRedirect
# from django.template import loader
from django.shortcuts import render, get_object_or_404
from django.http import Http404
from django.urls import reverse

from .models import Question, Choice

def index(request):
    print("polls haduken")
    print(request)
    # questions = Question.objects.all()
    # texto = ", ".join([str(q) for q in questions])
    latest_question_list = Question.objects.order_by('-pub_date')[:5]
    output = ', '.join([q.question_text for q in latest_question_list])

    # template = loader.get_template('polls/index.html')
    context = {
        'latest_question_list': latest_question_list,
    }
    # return HttpResponse(output)
    # return HttpResponse(template.render(context, request))
    return render(request, 'polls/index.html', context)

# Create your views here.

def detail(request, question_id):
    try:
        question = Question.objects.get(pk=question_id)
    except Question.DoesNotExist as dne:
        raise Http404("Question does not exist. Questão não existe")
    return render(request, 'polls/detail.html', {'question': question})
    # return HttpResponse("You're looking at question %s." % question_id)

def results(request, question_id):
    try:
        question = Question.objects.get(pk=question_id)
    except Question.DoesNotExist as dne:
        # raise Http404("Questão não existe {}".format(dne.message()))
        raise Http404("Questão não existe {}".format(dne))
    # response = "You're looking at the results of question: %s."
    return render(request, 'polls/results.html', {'question': question})
    # return HttpResponse(response % question)
    # return HttpResponse(response % question_id)

def vote(request, question_id):
    print("votaaaar")
    question = get_object_or_404(Question, pk=question_id)
    print(question)
    try:
        # question = Question.objects.get(pk=question_id)
        selected_choice = question.choice_set.get(pk=request.POST['choice'])
    except (KeyError, Choice.DoesNotExist):
        # Redisplay de question voting form.
        return render(request, 'polls/detail.html', {
            'question': question, 
            'error_message': "You didn't select a choice.",
        })
    else:
        selected_choice.votes += 1
        selected_choice.save()
        return HttpResponseRedirect(reverse('polls:results', args=(question.id,)))

# def vote_on_choice(request, question_id, choice):
#     print("votando vote_on_choice" % choice)
#     return HttpResponse("You're voting for choice {} on question {}.".format(choice, question_id))

def graph_results(request, question_id):
	print("executando graph_results")
	import matplotlib
	from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
	from matplotlib.figure import Figure
	from matplotlib.dates import DateFormatter
	fig = Figure()

	ax = fig.add_subplot(1,1,1)
	question = get_object_or_404(Question, pk=question_id) # Get the question object
	
	x = matplotlib.numpy.arange(1, question.choice_set.count())
	choices = question.choice_set.all()

	votes = [choice.votes for choice in choices]
	names = [choice.choice_text for choice in choices]

	numTests = question.choice_set.count()
	ind = matplotlib.numpy.arange(numTests) # the x locations for the groups

	cols = ['red', 'orange', 'yellow', 'green', 'blue', 'purple', 'indigo']*10

	cols = cols[0:len(ind)]
	ax.bar(ind, votes, color=cols)

	ax.set_xticks(ind + 0.5)
	ax.set_xticklabels(names)

	ax.set_xlabel("Choices")
	ax.set_ylabel("Votes")

	ax.grid(True)

	title = "Dynamically Generated Results Plot \nfor question: %s" % question.question_text
	ax.set_title(title)

	canvas = FigureCanvas(fig)

	# from django import settings
	from django.conf import settings
	# print("importado")
	# print(type(settings.BASE_DIR))
	# print(settings.BASE_DIR)

	# import os
	# img_path = os.path.join(settings.BASE_DIR, '/polls/static/polls/images/')
	img_path = settings.BASE_DIR + '/polls/static/polls/images/'
	# img_path.join('/polls/static/polls/images/')
	img_path += 'question{}.png'.format(question.id)

	# print("img_path ", img_path)
	# from . import urls
	# print("app_name ", app_name)

	fig.savefig(img_path) # salva a figura   savefig('foo.png', bbox_inches='tight')

	# sleep(5)

	# https://docs.python.org/3/library/io.html
	# from io import BytesIO
	# bio = BytesIO()
	# canvas.print_png(bio)
	# plot = bio.getvalue()
	# return HttpResponse(plot, content_type='image/png')


	# return render(request, 'polls/results.html', {'question': question, 'plot', plot})
	return render(request, 'polls/results.html', {'question': question,})