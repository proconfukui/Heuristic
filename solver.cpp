
void move_pair1(int target_entity, int goal_x, int goal_y,int layer)
{

    int current_x1 = pair_coordinates[target_entity][0];
    int current_y1 = pair_coordinates[target_entity][1];
    int current_x2 = pair_coordinates[target_entity][2];
    int current_y2 = pair_coordinates[target_entity][3];

    int target_pair_x, target_pair_y;
    int goal_pair_x = goal_x + 1;
    int goal_pair_y = goal_y;

    // goal_x, goal_y にあるのが1つ目と仮定し、2つ目を動かす
    if (current_x1 == goal_x && current_y1 == goal_y)
    {
        target_pair_x = current_x2;
        target_pair_y = current_y2;
    }
    else // goal_x, goal_y にあるのが2つ目と仮定し、1つ目を動かす
    {
        target_pair_x = current_x1;
        target_pair_y = current_y1;
    }

    if (goal_pair_x == target_pair_x && goal_pair_y == target_pair_y)
    {
        return;
    }

    // targetがgoalのすぐ下にある場合
    if (goal_pair_x - 1 == target_pair_x && goal_pair_y + 1 == target_pair_y)
    {
        rotate_field(goal_pair_x - 1, goal_pair_y, 2);
        return;
    }

    // targetが上に達して、かつgoalの右側にあるとき
    if (goal_pair_x < target_pair_x && goal_pair_y == target_pair_y)
    {
        int max_rotate_size;
        if (field_size - target_pair_y < target_pair_x - goal_pair_x)
        {
            max_rotate_size = field_size;
        }
        else
        {
            max_rotate_size = target_pair_x - goal_pair_x + 1;
        }
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size);
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size);
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y, max_rotate_size);
        return;
    }

    // targetがgoalの左に達した場合
    if (goal_pair_x == target_pair_x)
    {
        if (goal_pair_y == target_pair_y)
            return;
        int max_roteta_size;
        if (field_size - layer - target_pair_x < target_pair_y - goal_pair_y + 1 )
        {
            max_roteta_size = field_size - target_pair_x -layer;
        }
        else
        {
            max_roteta_size = target_pair_y - goal_pair_y + 1;
        }
        // targetが端にあるときのための処理
        if (target_pair_x + 1  == field_size -layer){
            rotate_field(target_pair_x - 1, target_pair_y - 1, 2);
            return;
        }
        rotate_field(target_pair_x, target_pair_y - max_roteta_size + 1, max_roteta_size);
        return;
    }

    // targetが上に達して、かつgoalの左側にあるとき
    if (goal_pair_x > target_pair_x && goal_pair_y + 1 == target_pair_y)
    {
        int max_rotate_size;
        if (field_size - target_pair_y < goal_pair_x - target_pair_x)
        {
            max_rotate_size = field_size - target_pair_y;
        }
        else
        {
            max_rotate_size = goal_pair_x - target_pair_x;
        }
        rotate_field(target_pair_x, target_pair_y, max_rotate_size);
        return;
    }

    // 動かしたいエンティティが目標の右下にある場合
    if (goal_pair_x < target_pair_x && goal_pair_y < target_pair_y)
    {
        int max_rotate_size;
        if (target_pair_x - goal_pair_x < target_pair_y - goal_pair_y)
        {
            max_rotate_size = target_pair_x - goal_pair_x + 1;
        }
        else
        {
            max_rotate_size = target_pair_y - goal_pair_y + 1;
        }
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y - max_rotate_size + 1, max_rotate_size);
        return;
    }
    // 動かしたいエンティティが目標の左下にある場合
    if (goal_pair_x > target_pair_x && goal_pair_y < target_pair_y)
    {
        int max_rotate_size;
        if (goal_pair_x - target_pair_x < target_pair_y - goal_pair_y)
        {
            max_rotate_size = goal_pair_x - target_pair_x + 1;
        }
        else
        {
            max_rotate_size = target_pair_y - goal_pair_y;
        }
        rotate_field(target_pair_x, target_pair_y - max_rotate_size + 1, max_rotate_size);
        return;
    }
}

void solve1()
{

    for (int i = 0; i < field_size/2; i += 2)
    {
        for (int x = i; x < field_size-i; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i);
                print_field();
            }
        }
        for (int x = i; x < field_size-i; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i+1,i);
                print_field();
            }
        }
        rotate_field(0, 0, field_size);
        print_field();
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i+2);
                print_field();
            }
        }
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i + 1,i+2);
                print_field();
            }
        }
        rotate_field(0, 0, field_size);
        print_field();
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i+2);
                print_field();
            }
        }
        for (int x = i ; x < field_size-i-2; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i + 1,i+2);
                print_field();
            }
        }
        rotate_field(0, 0, field_size);
        print_field();
        for (int x = i +2; x < field_size-i-2; x += 2)
        {
            while (field[i][x] != field[i][x + 1])
            {
                move_pair1(field[i][x], x, i,i+2);
                print_field();
            }
        }
        for (int x = i +2; x < field_size-i-2; x += 2)
        {
            while (field[i + 1][x] != field[i + 1][x + 1])
            {
                move_pair1(field[i + 1][x], x, i + 1,i+2);
                print_field();
            }
        }
    }
}
