#include "project.h"

#include <QApplication>
#include <QDateTime>
#include <QPixmap>


namespace Domain
{

class Project::Implementation
{
public:
    ProjectType type = ProjectType::Invalid;
    QString path = "/home/lucas/screenplays/tlj.starc";

    mutable QPixmap poster;
    QString posterPath;
    QString name = "Star Wars";
    QString logline = "Rey develops her newly discovered abilities with the guidance of Luke Skywalker, who is unsettled by the strength of her powers. Meanwhile, the Resistance prepares for battle with the First Order. ";
    QDateTime lastEditTime = QDateTime::fromString("2019-10-01 22:48:56", "yyyy-MM-dd hh:mm:ss");
};


// **


Project::Project()
    : d(new Implementation)
{
}

Project::Project(const Project& _other)
    : d(new Implementation(*_other.d))
{
}

const Project& Project::operator=(const Project& _other)
{
    d.reset(new Implementation(*_other.d));
    return *this;
}

bool Project::operator==(const Project& _other)
{
    return type() == _other.type()
            && path() == _other.path()
            && posterPath() == _other.posterPath()
            && name() == _other.name()
            && logline() == _other.logline()
            && lastEditTime() == _other.lastEditTime();
}

Project::~Project() = default;

ProjectType Project::type() const
{
    return d->type;
}

void Project::setType(ProjectType _type)
{
    d->type = _type;
}

QString Project::name() const
{
    return d->name;
}

void Project::setName(const QString& _name)
{
    d->name = _name;
}

QString Project::logline() const
{
    return d->logline;
}

void Project::setLogline(const QString& _logline)
{
    d->logline = _logline;
}

QString Project::displayLastEditDate() const
{
    switch (d->lastEditTime.daysTo(QDateTime::currentDateTime())) {
        case 0: {
            return QApplication::translate("Domain::Project", "today at") + d->lastEditTime.toString(" hh:mm");
        }

        case 1: {
            return QApplication::translate("Domain::Project", "tomorrow at") + d->lastEditTime.toString(" hh:mm");
        }

        default: {
            return d->lastEditTime.toString("dd.MM.yyyy hh:mm");
        }
    }
}

QString Project::displayPath() const
{
    //
    // TODO: Remote project path
    //
    return d->path;
}

QString Project::path() const
{
    return d->path;
}

void Project::setPath(const QString& _path)
{
    d->path = _path;
}

const QPixmap& Project::poster() const
{
    if (d->poster.isNull()) {
        if (!d->poster.load(d->posterPath)) {
            static const QPixmap kDefaultPoster(":/images/movie-poster");
            d->poster = kDefaultPoster;
        }
    }

    return d->poster;
}

QString Project::posterPath() const
{
    return d->posterPath;
}

void Project::setPosterPath(const QString& _path)
{
    d->posterPath = _path;
}

QDateTime Project::lastEditTime() const
{
    return d->lastEditTime;
}

void Project::setLastEditTime(const QDateTime& _time)
{
    d->lastEditTime = _time;
}

QVariant Project::data(int _role) const
{
    switch (_role) {
        case ProjectDataRole::Type: {
            return static_cast<int>(type());
        }

        case ProjectDataRole::Path: {
            return path();
        }

        case ProjectDataRole::PosterPath: {
            return posterPath();
        }

        case ProjectDataRole::Name: {
            return name();
        }

        case ProjectDataRole::Logline: {
            return logline();
        }

        case ProjectDataRole::LastEditTime: {
            return lastEditTime();
        }

        default: {
            return {};
        }
    }
}


// ****


class ProjectsModel::Implementation
{
public:
    QVector<Project> projects;
};


// **


ProjectsModel::ProjectsModel(QObject* _parent)
    : QAbstractListModel(_parent),
      d(new Implementation)
{
}

Project ProjectsModel::projectAt(int _row) const
{
    if (_row < 0
        || _row >= d->projects.size()) {
        return {};
    }

    return d->projects.at(_row);
}

ProjectsModel::~ProjectsModel() = default;

void ProjectsModel::addProject(const QString& _name)
{
    beginInsertRows({}, 0, 0);
    Project project;
    project.setName(_name);
    d->projects.prepend(project);
    endInsertRows();
}

int ProjectsModel::rowCount(const QModelIndex& _parent) const
{
    Q_UNUSED(_parent);
    return d->projects.size();
}

QVariant ProjectsModel::data(const QModelIndex& _index, int _role) const
{
    if (!_index.isValid()) {
        return {};
    }

    const int projectIndex = _index.row();
    if (projectIndex < 0
        || projectIndex >= d->projects.size()) {
        return {};
    }

    const Project& project = d->projects.at(projectIndex);
    return project.data(_role);
}

} // namespace Domain